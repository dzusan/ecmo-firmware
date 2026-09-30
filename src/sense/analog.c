#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "stm32f7xx_hal.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "cbuf.h"
#include "analog.h"

/*
 * 1 - yellow(old), green(new) - 24 V
 * 2 - green(old), white(new) - signal
*/

/* Differentiation threshold constant in ADC values
* 35 ADC ~ 350 kPa */

// #define DIFF_TH              (5000)

#define THREAD_STACK_SIZE_ANALOG (256)   /* Stack size in words, not bytes */
#define CBUF_LEN (40)

const uint32_t wait_time_ms_k = 100;

static SemaphoreHandle_t semphr = NULL;
static TaskHandle_t analog_handle = NULL;
static struct analog_channel
{
    struct cbuf rawbuf;
    uint16_t storage[CBUF_LEN];
    uint32_t median;
} adc_ch[ADC_CH_TOTAL] = {0};

int16_t analog_get(const enum adc_channel id)
{
    if (id >= ADC_CH_TOTAL)
    {
        return 0;
    }

    int32_t raw = 0;
    int64_t result = 0;

    const int32_t k_uv_msb = 3270000 / 4096; // uV [TESTED 2025-10-24: IDEAL!]

    const int32_t k_zero_pres_cur = 4000; // uA
    const int32_t k_max_pres_cur = 20000; // uA
    const int32_t k_cur_full_scale = k_max_pres_cur - k_zero_pres_cur; // uA
    const int32_t k_full_p1 = 250000; // Pa
    const int32_t k_full_p2 = 160000; // Pa
    const int32_t k_bias_p1 = 100000; // Pa

    // rdiv_h = 82; show 23.88, fact 24.11
    // rdiv_h = 83; show 24.11, fact 24.11
    const int32_t rdiv_h = 83;
    const int32_t rdiv_l = 10;

    BaseType_t ret = xSemaphoreTake(semphr, wait_time_ms_k);

    if (ret == pdTRUE)
    {
        raw = (int32_t)adc_ch[id].median;
        (void)xSemaphoreGive(semphr);
    }

    switch (id)
    {
        case ADC_CH_SEN_PRES_1:
            result = raw * k_uv_msb / 160; // uA [TESTED 2025-10-24: IDEAL!]
            result = result - k_zero_pres_cur;

            if (result < 0)
            {
                return ANALOG_INVALID_VAL;
            }

            result = result * k_full_p1 / k_cur_full_scale;
            result = result - k_bias_p1;
            result = result / 133; // mmHg
            return (int16_t)result;

        case ADC_CH_SEN_PRES_2:
            result = raw * k_uv_msb / 160; // uA [TESTED 2025-10-24: IDEAL!]
            result = result - k_zero_pres_cur;

            if (result < 0)
            {
                return ANALOG_INVALID_VAL;
            }

            result = result * k_full_p2 / k_cur_full_scale;
            result = result / 133; // mmHg
            return (int16_t)result;

        case ADC_CH_SEN_V_SUP:
            result = raw * k_uv_msb;
            result = result * (rdiv_h + rdiv_l) / rdiv_l / 1000;
            return (int16_t)result;

        case ADC_CH_SEN_U_I:
            result = raw * k_uv_msb / 1000; // mA (1V/A) [TESTED 2025-10-24: IDEAL!]
            return (int16_t)result;

        default:
            return 0;
    }

    if (result < 0)
    {
        result = 0;
    }

    return (int16_t)result;
}

int cmpfunc(const void* a, const void* b)
{
    int16_t x = (int16_t)(*(uint16_t*)a);
    int16_t y = (int16_t)(*(uint16_t*)b);
    return x - y;
}

static void analog_thread(void* unused)
{
    enum adc_channel id = 0;

    uint16_t sort_storage[CBUF_LEN];
    uint16_t result;
    uint32_t sig;

    BaseType_t ret = xSemaphoreGive(semphr);
    assert(ret == pdTRUE);

    for (;;)
    {
        sig = 0;
        int status = adc_start(id);

        if (status < 0)
        {
            vTaskDelay(1);
            continue;
        }

        xTaskNotifyWait(0, ULONG_MAX, &sig, portMAX_DELAY);

        if (sig != 0)
        {
            result = adc_get(id);

            ret = xSemaphoreTake(semphr, wait_time_ms_k);

            if (ret == pdTRUE)
            {

#ifdef DIFF_TH

                /* Differentiation */
                int32_t diff = (int32_t)result - (int32_t)adc_ch[id].median;

                if (abs(diff) > DIFF_TH)
                {
                    adc_ch[id].median = result;

                    for (int i = 0; i < CBUF_LEN; i++)
                    {
                        adc_ch[id].rawbuf.data[i] = result;
                    }
                }
                else

#endif

                {
                    cbuf_add(&adc_ch[id].rawbuf, &result);

                    /* Median filter */
                    memcpy(sort_storage, adc_ch[id].rawbuf.data, sizeof(sort_storage));
                    qsort(sort_storage, CBUF_LEN, sizeof(sort_storage[0]), cmpfunc);

                    /* Average */
                    int32_t acc = 0;
                    const uint32_t padding = CBUF_LEN / 10;

                    for (uint32_t i = padding; i < CBUF_LEN - padding; i++)
                    {
                        acc += sort_storage[i];
                    }

                    adc_ch[id].median = acc / (CBUF_LEN - padding * 2);
                }

                (void)xSemaphoreGive(semphr);
            }

            id++;

            if (id >= ADC_CH_TOTAL)
            {
                id = 0;
            }
        }
        else
        {
            vTaskDelay(1);
        }
    }
}

void adc_irq(void)
{
    if (analog_handle != NULL)
    {
        xTaskNotifyFromISR(analog_handle, 1, eSetValueWithOverwrite, NULL);
    }
}

void analog_init(void)
{
    for (uint8_t i = 0; i < ADC_CH_TOTAL; i += 1)
    {
        adc_ch[i].rawbuf.data = (uint8_t*)adc_ch[i].storage;
        adc_ch[i].rawbuf.item_size = sizeof(adc_ch[i].storage[0]);
        adc_ch[i].rawbuf.total_items = CBUF_LEN;
    }

    static StaticSemaphore_t semaphore_buf;
    semphr = xSemaphoreCreateBinaryStatic(&semaphore_buf);
    assert(semphr != NULL);

    static StackType_t analog_stack[THREAD_STACK_SIZE_ANALOG];
    static StaticTask_t analog_ctl;

    analog_handle = xTaskCreateStatic(analog_thread,
            "analog_thread",
            THREAD_STACK_SIZE_ANALOG,
            NULL,
            tskIDLE_PRIORITY,
            analog_stack,
            &analog_ctl);

    assert(analog_handle != NULL);
}
