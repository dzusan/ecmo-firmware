#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "stm32f7xx_hal.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "temperature.h"

static QueueHandle_t queue[TEMPERATURE_CH_TOTAL] = {0};

double temperature_get(enum temperature_ch ch)
{
    if (ch >= TEMPERATURE_CH_TOTAL)
    {
        return 0;
    }

    uint32_t cnt = 0;
    xQueuePeek(queue[ch], &cnt, 0);

    if (cnt < 2)
    {
        return TEMPERATURE_INVALID_VAL;
    }

    double val = cnt * 0.0625 - 50.0625;
    return val;
}

void temperature_irq(enum temperature_ch ch)
{
    if (ch >= TEMPERATURE_CH_TOTAL)
    {
        return;
    }

    static uint32_t cnt[TEMPERATURE_CH_TOTAL] = {0};
    static uint32_t time_past[TEMPERATURE_CH_TOTAL] = {0};

    const uint32_t timeout_ms_k = 16; /* Tconv min */
    uint32_t time_now = xTaskGetTickCountFromISR();

    if (time_now - time_past[ch] > timeout_ms_k)
    {
        BaseType_t woken = pdFALSE;
        xQueueOverwriteFromISR(queue[ch], &cnt[ch], &woken);
        cnt[ch] = 0;
        time_past[ch] = time_now;
    }

    cnt[ch] += 1;
}

void digin_irq_flow_f(void)
{
    temperature_irq(TEMPERATURE_CH_MOT);
}

void digin_irq_flow_r(void)
{
    temperature_irq(TEMPERATURE_CH_ENV);
}

void temperature_init(void)
{
    static StaticQueue_t queue_ctl[TEMPERATURE_CH_TOTAL];
    static uint32_t storage[TEMPERATURE_CH_TOTAL];

    for (int i = 0; i < TEMPERATURE_CH_TOTAL; i++)
    {
        queue[i] = xQueueCreateStatic(
                1,
                sizeof(storage[i]),
                (uint8_t*) & (storage[i]),
                &queue_ctl[i]
            );
    }
}
