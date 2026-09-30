#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "stm32f7xx_hal.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "cbuf.h"
#include "digin.h"
#include "flow.h"

#define THREAD_STACK_SIZE_ANALOG (512)   /* Stack size in words, not bytes */
#define CBUF_LEN (20) /* 20 seconds */

/*
F -> R
R -> C
C -> F
*/

#define SIG_FORWARD  (DIGIN_FLOW_C)

static TaskHandle_t flow_handle = NULL;
static QueueHandle_t uso_queue = NULL;
static QueueHandle_t rot_queue = NULL;
static QueueHandle_t mode_queue = NULL;

static void flow_thread(void* unused)
{
    const uint32_t pulse_width_min_ms_k = 31;
    const uint32_t uso_ml_per_tick_k = 100;
    static uint32_t uso_storage[CBUF_LEN] = {0};
    struct cbuf uso_buf =
    {
        .data = (uint8_t*)uso_storage,
        .item_size = sizeof(uso_storage[0]),
        .total_items = CBUF_LEN,
    };

    const uint32_t rot_ticks_per_l_k = 402;
    static uint32_t rot_storage[CBUF_LEN] = {0};
    struct cbuf rot_buf =
    {
        .data = (uint8_t*)rot_storage,
        .item_size = sizeof(rot_storage[0]),
        .total_items = CBUF_LEN,
    };

    const uint32_t poll_period_ms_k = 1;
    const uint32_t upd_period_ms_k = 1000;
    uint32_t uso_acc = 0;
    uint32_t rot_acc = 0;
    uint32_t prev_time = xTaskGetTickCount();

    for (;;)
    {
        vTaskDelay(poll_period_ms_k);

        rot_acc += digin_proc_front(DIGIN_FLOW_ROT);
        uint32_t cur_time = xTaskGetTickCount();

        if (cur_time - prev_time >= upd_period_ms_k)
        {
            prev_time = cur_time;
            enum flow_mode mode = FLOW_MODE_FLOW;
            BaseType_t res = xQueuePeek(mode_queue, &mode, 0);

            if (res == pdTRUE && mode == FLOW_MODE_COUNT)
            {
                xQueueOverwrite(uso_queue, &uso_acc);
                xQueueOverwrite(rot_queue, &rot_acc);
            }
            else
            {
                cbuf_add(&uso_buf, &uso_acc);
                cbuf_add(&rot_buf, &rot_acc);
                uso_acc = 0;
                rot_acc = 0;

                for (int i = 0; i < CBUF_LEN; i += 1)
                {
                    uso_acc += uso_storage[i];
                    rot_acc += rot_storage[i];
                }

                uint32_t total_ml = uso_acc * uso_ml_per_tick_k;
                /* ml / part_of_minute = ml / (1/multiplier) = ml * multiplier */
                uint32_t ml_per_minute =
                    total_ml * 1000 * 60 / (upd_period_ms_k * CBUF_LEN);

                uint32_t th_sqm_per_hour = ml_per_minute * 60 / 1000;
                uint32_t l_per_minute = ml_per_minute / 1000;
                xQueueOverwrite(uso_queue, &ml_per_minute);

                /* (ml per tick) = (1000 ml / rot_ticks_per_l_k) */
                total_ml = rot_acc * 1000 / rot_ticks_per_l_k;
                ml_per_minute =
                    total_ml * 1000 * 60 / (upd_period_ms_k * CBUF_LEN);
                xQueueOverwrite(rot_queue, &ml_per_minute);

                uso_acc = 0;
                rot_acc = 0;
            }

        }
    }
}

uint32_t flow_get_uso(void)
{
    uint32_t val = 0;
    xQueuePeek(uso_queue, &val, 0);
    return val;
}

uint32_t flow_get_rot(void)
{
    uint32_t val = 0;
    xQueuePeek(rot_queue, &val, 0);
    return val;
}

void flow_set_mode(enum flow_mode mode)
{
    xQueueOverwrite(mode_queue, &mode);
}

void flow_init(void)
{
    static StaticQueue_t queue_ctl;
    static uint32_t storage;
    uso_queue =
        xQueueCreateStatic(1, sizeof(storage), (uint8_t*)&storage, &queue_ctl);

    static StaticQueue_t queue_ctl_rot;
    static uint32_t storage_rot;
    rot_queue =
        xQueueCreateStatic(1, sizeof(storage_rot), (uint8_t*)&storage_rot,
        &queue_ctl_rot);

    static StaticQueue_t queue_ctl_mode;
    static enum flow_mode storage_mode;
    mode_queue =
        xQueueCreateStatic(1, sizeof(storage_mode), (uint8_t*)&storage_mode,
        &queue_ctl_mode);

    static StackType_t flow_stack[THREAD_STACK_SIZE_ANALOG];
    static StaticTask_t flow_ctl;
    flow_handle = xTaskCreateStatic(flow_thread,
            "flow_thread",
            THREAD_STACK_SIZE_ANALOG,
            NULL,
            tskIDLE_PRIORITY,
            flow_stack,
            &flow_ctl);

    assert(flow_handle != NULL);
}
