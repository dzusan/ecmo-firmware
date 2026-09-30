#include <assert.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include "mon.h"
#include "bsp_gpio.h"

#define THREAD_STACK_SIZE_MON (256)   /* Stack size in words, not bytes */

const uint32_t k_delay = 500;
static int values[MON_TOTAL] = {0};
static SemaphoreHandle_t mutex = NULL;
static TaskHandle_t mon_handle = NULL;

static void mon_thread(void* arg)
{
    (void)arg;

    for (;;)
    {
        uint32_t sig = 0;
        xTaskNotifyWait(0, ULONG_MAX, &sig, k_delay);

        if (xSemaphoreTake(mutex, k_delay) == pdTRUE)
        {
#ifdef MC_DEBUG
            printf("%d, %d, %d, %d\n",
                values[MON_MC_STAGE],
                values[MON_RPM],
                values[MON_MC_PWM],
                values[MON_MC_ROT_CNT]
            );
#else
            printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                values[MON_BTN],
                values[MON_RPM],
                values[MON_FLOW_ROT],
                values[MON_PD],
                values[MON_P1],
                values[MON_P2],
                values[MON_MC_I],
                values[MON_MC_P],
                values[MON_U],
                values[MON_TM],
                values[MON_TO],
                xTaskGetTickCount()
            );
#endif
            xSemaphoreGive(mutex);
        }

        bsp_gpio_led(true);
        vTaskDelay(k_delay / 5);
        bsp_gpio_led(false);
    }
}

int mon_init(void)
{
    static StaticSemaphore_t mutex_buf;
    mutex = xSemaphoreCreateMutexStatic(&mutex_buf);
    assert(mutex != NULL);

    static StackType_t mon_stack[THREAD_STACK_SIZE_MON];
    static StaticTask_t mon_ctl;

    mon_handle = xTaskCreateStatic(mon_thread,
            "mon_thread",
            THREAD_STACK_SIZE_MON,
            NULL,
            tskIDLE_PRIORITY,
            mon_stack,
            &mon_ctl);

    assert(mon_handle    != NULL);
}

int mon_write(enum mon_id id, int val)
{
    if (id >= MON_TOTAL)
    {
        return -ENOENT;
    }

    if (xSemaphoreTake(mutex, k_delay) == pdTRUE)
    {
        values[id] = val;
        xSemaphoreGive(mutex);
    }

    return 0;
}

void mon_trig(void)
{
    if (mon_handle != NULL)
    {
        xTaskNotify(mon_handle, 1, eSetValueWithOverwrite);
    }
}
