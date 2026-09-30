#include <assert.h>
#include <limits.h>
#include <FreeRTOS.h>
#include <task.h>
#include <lvgl.h>
#include "pwm.h"
#include "bsp_gpio.h"
#include "battery.h"

#define THREAD_STACK_SIZE_BATTERY (256)   /* Stack size in words, not bytes */

static TaskHandle_t battery_handle = NULL;

static void battery_thread(void* arg)
{
    (void)arg;

    pwm_set(PWMCH_PUMP_SUP, pwm_middle_k);
    pwm_set(PWMCH_PUMP_BAT, pwm_middle_k);
    pwm_set(PWMCH_FAN, 210);

    // pwm_set(PWMCH_MOTOR_V, pwm_middle_k);
    // pwm_set(PWMCH_MOTOR_U, pwm_middle_k);
    // pwm_set(PWMCH_MOTOR_W, pwm_middle_k);

    for (;;)
    {
        vTaskDelay(500);

        bsp_gpio_bkp_sup(true);
        bsp_gpio_bkp_bat(true);
        // pwm_set(PWMCH_FAN, 0);

        vTaskDelay(500);

        bsp_gpio_bkp_sup(false);
        bsp_gpio_bkp_bat(false);
        // pwm_set(PWMCH_FAN, pwm_level_max_k - 1);
    }
}

int battery_init(void)
{
    static StackType_t battery_stack[THREAD_STACK_SIZE_BATTERY];
    static StaticTask_t battery_ctl;
    battery_handle = xTaskCreateStatic(battery_thread,
            "battery_thread",
            THREAD_STACK_SIZE_BATTERY,
            NULL,
            tskIDLE_PRIORITY,
            battery_stack,
            &battery_ctl);

    assert(battery_handle != NULL);
}
