#include <assert.h>
#include <limits.h>
#include <cmsis_compiler.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <lvgl.h>
#include "mon.h"
#include "hall.h"
#include "pwm.h"
#include "pid.h"
#include "mc_init.h"
#include "speaker.h"

#include <stdio.h>

#define THREAD_STACK_SIZE_MC (256)   /* Stack size in words, not bytes */

const int32_t speed_max_k = 60000;
const int32_t speed_min_k = 6000;
const uint32_t idle_time_k = 200;
const uint32_t esc_poweron_time_k = 7000;

enum operation_stage
{
    STALL,
    SPEEDUP,
    PID,
};

struct shared_data
{
    bool enable;
    uint32_t step_cnt;
};

static struct shared_data shared = {0};
static SemaphoreHandle_t semphr = NULL;
static QueueHandle_t queue_actual = NULL;
static QueueHandle_t queue_setting = NULL;

static void ready_beep(void)
{
    const uint32_t duration_k = 50;

    for (int i = 0; i < 3; i++)
    {
        speaker_set_tone(SPK_TONE_MIDDLE, 100);
        vTaskDelay(duration_k);
        speaker_set_tone(0, 0);
        vTaskDelay(duration_k);
    }
}

static void mc_init_thread(void* arg)
{
    (void)arg;

    /* PID controller */
    double I = 0.0;
    double er = 0.0;

    BaseType_t ret = xSemaphoreGive(semphr);
    assert(ret == pdTRUE);

    pwm_set(PWMCH_ESC, 0);
    vTaskDelay(esc_poweron_time_k);

    ret = xSemaphoreTake(semphr, idle_time_k);

    if (ret == pdTRUE)
    {
        shared.enable = false;
        shared.step_cnt = 0;
        (void)xSemaphoreGive(semphr);
    }

    ready_beep();

    uint32_t rot_cnt_prev = 0;
    uint32_t pwm_level = 0;
    enum operation_stage stage = 0;

    for (;;)
    {
        int32_t rpm_fact = 0;
        const uint32_t sec_in_min = 60;
        const uint32_t ms_in_sec = 1000;

        ret = xSemaphoreTake(semphr, idle_time_k);

        if (ret == pdTRUE)
        {
            const uint32_t rot_stall = speed_min_k * pid_period_ms_k / sec_in_min /
                ms_in_sec;
            rpm_fact = shared.step_cnt * sec_in_min * ms_in_sec / pid_period_ms_k;

            if (shared.enable)
            {
                if (shared.step_cnt <= rot_stall)
                {
                    stage = STALL;
                }

                switch (stage)
                {
                    case STALL:
                        I = 0.0; er = 0.0; /* PID controller reset */
                        pwm_level = pwm_min_k;
                        pwm_set(PWMCH_ESC, pwm_min_k);

                        if (shared.step_cnt > rot_stall)
                        {
                            stage = SPEEDUP;
                        }

                        break;

                    case SPEEDUP:
                        I = 0.0; er = 0.0; /* PID controller reset */
                        pwm_level = pwm_min_k;
                        pwm_set(PWMCH_ESC, pwm_min_k);

                        if ((shared.step_cnt <= rot_cnt_prev))
                        {
                            stage = PID;
                        }

                        break;

                    case PID:
                        break;

                    default:
                        I = 0.0; er = 0.0; /* PID controller reset */
                        pwm_level = pwm_min_k;
                        pwm_set(PWMCH_ESC, pwm_min_k);
                        stage = STALL;
                        break;
                }
            }
            else
            {
                pwm_level = 0;
                stage = STALL;
                pwm_set(PWMCH_ESC, 0);
            }

            rot_cnt_prev = shared.step_cnt;
            shared.step_cnt = 0;

            (void)xSemaphoreGive(semphr);
        }

        if (stage == PID)
        {
            /* RPM by one tick */
            const uint32_t rpm_tolerance = sec_in_min * ms_in_sec / pid_period_ms_k;

            int32_t rpm_setting = 0;
            xQueuePeek(queue_setting, &rpm_setting, 0);

            /* Empirically */
            uint32_t rpm_operating_diff = rpm_tolerance + (rpm_setting - speed_min_k) / 2;

            bool is_adjust = false;

            if (rpm_fact > rpm_setting)
            {
                if (rpm_fact - rpm_setting > rpm_tolerance)
                {
                    is_adjust = true;
                }
            }

#ifdef USE_SMOOTH_APPROACH
            else if (rpm_setting - rpm_fact > rpm_operating_diff)
            {
                pwm_level += 1;
                pwm_set(PWMCH_ESC, pwm_level);
            }

#endif
            else
            {
                if (rpm_setting - rpm_fact > rpm_tolerance)
                {
                    is_adjust = true;
                }
            }

            if (is_adjust)
            {
                pwm_level = pid_proc(rpm_setting, rpm_fact, speed_max_k, &I, &er);
                pwm_set(PWMCH_ESC, pwm_level);
            }
        }

        mon_write(MON_RPM, (int)rpm_fact);
        mon_write(MON_MC_PWM, (int)pwm_level);
        mon_write(MON_MC_STAGE, (int)stage);
        mon_write(MON_MC_ROT_CNT, (int)rot_cnt_prev);
        xQueueOverwrite(queue_actual, &rpm_fact);
        vTaskDelay(pid_period_ms_k);
    }
}

int mc_init(void)
{
    static StaticQueue_t queue_actual_ctl, queue_setting_ctl;
    static int32_t storage_actual, storage_setting;

    queue_actual = xQueueCreateStatic(
            1,
            sizeof(storage_actual),
            (uint8_t*)&storage_actual,
            &queue_actual_ctl
        );

    queue_setting = xQueueCreateStatic(
            1,
            sizeof(storage_setting),
            (uint8_t*)&storage_setting,
            &queue_setting_ctl
        );

    static StaticSemaphore_t semaphore_buf;
    semphr = xSemaphoreCreateBinaryStatic(&semaphore_buf);
    assert(semphr != NULL);

    static TaskHandle_t mc_init_handle = NULL;
    static StackType_t mc_init_stack[THREAD_STACK_SIZE_MC];
    static StaticTask_t mc_init_ctl;
    mc_init_handle = xTaskCreateStatic(mc_init_thread,
            "mc_init_thread",
            THREAD_STACK_SIZE_MC,
            NULL,
            tskIDLE_PRIORITY + 5,
            mc_init_stack,
            &mc_init_ctl);

    assert(mc_init_handle != NULL);
}

void mc_enable(bool en)
{
    pwm_set(PWMCH_ESC, 0);

    BaseType_t ret = xSemaphoreTake(semphr, idle_time_k);

    if (ret == pdTRUE)
    {
        shared.enable = en;
        shared.step_cnt = 0;
        (void)xSemaphoreGive(semphr);
    }
}

int32_t mc_get_speed(void)
{
    int32_t actual = 0;
    xQueuePeek(queue_actual, &actual, 0);
    return actual;
}

int32_t mc_get_max(void)
{
    return speed_max_k;
}

int32_t mc_set_speed(int32_t speed)
{
    if (speed > speed_max_k)
    {
        speed = speed_max_k;
    }
    else if (speed < speed_min_k)
    {
        speed = speed_min_k;
    }

    xQueueOverwrite(queue_setting, &speed);
    return speed;
}

void digin_irq_esc_fb(void)
{
    if (semphr != NULL)
    {
        BaseType_t ret = xSemaphoreTakeFromISR(semphr, NULL);

        if (ret == pdTRUE)
        {
            if (shared.enable)
            {
                shared.step_cnt += 1;
            }

            (void)xSemaphoreGiveFromISR(semphr, NULL);
        }
    }
}

void hall_irq(enum hall_angle angle)
{
    (void)angle;
}
