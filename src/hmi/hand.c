#include <assert.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "keyboard.h"
#include "mon.h"
#include "hmi.h"
#include "digin.h"
#include "hand.h"

#define THREAD_STACK_SIZE_HAND (300)   /* Stack size in words, not bytes */

static void hand_thread(void* arg)
{
    (void)arg;
    const uint32_t delay_k = 50;
    uint16_t fixed = 0xFFFF;
    uint16_t read = 0;
    uint16_t changed = 0;
    uint16_t value = 0;

    for (;;)
    {
        vTaskDelay(delay_k);
        read = keyboard_read();
        changed = fixed ^ read;

        if (changed)
        {
            vTaskDelay(delay_k);
            read = keyboard_read();

            if (changed == (fixed ^ read))
            {
                fixed ^= changed;

                if (changed & KEYBOARD_CANCEL)
                {
                    if (fixed & KEYBOARD_CANCEL)
                    {
                        hmi_kb_cb(KEYBOARD_CANCEL, false);
                        // printf("CANCEL released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_CANCEL, true);
                        // printf("CANCEL pressed\n");
                    }
                }
                else if (changed & KEYBOARD_UP)
                {
                    if (fixed & KEYBOARD_UP)
                    {
                        hmi_kb_cb(KEYBOARD_UP, false);
                        // printf("UP released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_UP, true);
                        // printf("UP pressed\n");
                    }
                }
                else if (changed & KEYBOARD_MENU)
                {
                    if (fixed & KEYBOARD_MENU)
                    {
                        hmi_kb_cb(KEYBOARD_MENU, false);
                        // printf("MENU released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_MENU, true);
                        // printf("MENU pressed\n");
                    }
                }
                else if (changed & KEYBOARD_START)
                {
                    if (fixed & KEYBOARD_START)
                    {
                        hmi_kb_cb(KEYBOARD_START, false);
                        // printf("START released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_START, true);
                        // printf("START pressed\n");
                    }
                }
                else if (changed & KEYBOARD_LEFT)
                {
                    if (fixed & KEYBOARD_LEFT)
                    {
                        hmi_kb_cb(KEYBOARD_LEFT, false);
                        // printf("LEFT released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_LEFT, true);
                        // printf("LEFT pressed\n");
                    }
                }
                else if (changed & KEYBOARD_DOWN)
                {
                    if (fixed & KEYBOARD_DOWN)
                    {
                        hmi_kb_cb(KEYBOARD_DOWN, false);
                        // printf("DOWN released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_DOWN, true);
                        // printf("DOWN pressed\n");
                    }
                }
                else if (changed & KEYBOARD_RIGHT)
                {
                    if (fixed & KEYBOARD_RIGHT)
                    {
                        hmi_kb_cb(KEYBOARD_RIGHT, false);
                        // printf("RIGHT released\n");
                    }
                    else
                    {
                        hmi_kb_cb(KEYBOARD_RIGHT, true);
                        // printf("RIGHT pressed\n");
                    }
                }
            }
        }
    }
}

static void enc_thread(void* arg)
{
    (void)arg;
    const uint32_t delay_k = 5;
    const uint32_t btn_th_k = 20;
    uint8_t pending_a = 0;
    uint8_t pending_b = 0;
    uint8_t pending_push = 1;
    uint8_t pending_release = 0;
    uint8_t cnt_push = 0;
    uint8_t cnt_release = 0;
    int btn_state = 0;

    for (;;)
    {
        vTaskDelay(delay_k);

        if (digin_proc_front(DIGIN_ENC_A))
        {
            if (pending_b)
            {
                pending_b = 0;
                hmi_enc_cb(HMI_ENC_CCW);
                // printf("CCW\n");
            }
            else
            {
                pending_a = 1;
            }
        }
        else if (digin_get_state(DIGIN_ENC_A))
        {
            pending_a = 0;
        }

        if (digin_proc_front(DIGIN_ENC_B))
        {
            if (pending_a)
            {
                pending_a = 0;
                hmi_enc_cb(HMI_ENC_CW);
                // printf("CW\n");
            }
            else
            {
                pending_b = 1;
            }
        }
        else if (digin_get_state(DIGIN_ENC_B))
        {
            pending_b = 0;
        }

        btn_state = digin_get_state(DIGIN_ENC_BTN);

        if (btn_state == 0)
        {
            if (pending_release == 0)
            {
                cnt_push += 1;
            }
            else
            {
                cnt_release = 0;
            }
        }
        else if (btn_state != 0)
        {
            if (pending_push == 0)
            {
                cnt_release += 1;
            }
            else
            {
                cnt_push = 0;
            }
        }

        if (cnt_push > btn_th_k)
        {
            pending_push = 0;
            pending_release = 1;
            cnt_push = 0;
            cnt_release = 0;
            hmi_enc_cb(HMI_ENC_PRESSED);
            // printf("EBTN_PUSH\n");
        }

        if (cnt_release > btn_th_k)
        {
            pending_push = 1;
            pending_release = 0;
            cnt_push = 0;
            cnt_release = 0;
            hmi_enc_cb(HMI_ENC_RELEASED);
            // printf("EBTN_RELEASE\n");
        }
    }
}

int hand_init(void)
{
    static StackType_t hand_stack[THREAD_STACK_SIZE_HAND];
    static StaticTask_t hand_ctl;

    TaskHandle_t hand_handle = xTaskCreateStatic(hand_thread,
            "hand_thread",
            THREAD_STACK_SIZE_HAND,
            NULL,
            tskIDLE_PRIORITY,
            hand_stack,
            &hand_ctl);

    assert(hand_handle != NULL);

    static StackType_t enc_stack[THREAD_STACK_SIZE_HAND];
    static StaticTask_t enc_ctl;

    TaskHandle_t enc_handle = xTaskCreateStatic(enc_thread,
            "enc_thread",
            THREAD_STACK_SIZE_HAND,
            NULL,
            tskIDLE_PRIORITY,
            enc_stack,
            &enc_ctl);

    assert(enc_handle != NULL);
}
