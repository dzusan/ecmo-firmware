#include "stm32f7xx_hal.h"
#include "clock.h"
#include "bsp_gpio.h"
#include "hall.h"
#include "keyboard.h"
#include "pwm.h"
#include "adc.h"
#include "digin.h"
#include "speaker.h"
#include "bsp_init.h"

void bsp_init(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    clock_rcc_init();

    bsp_gpio_init();
    // hall_init();
    keyboard_init();
    pwm_init();
    adc_init();
    digin_init();
    speaker_init();

    bsp_gpio_pwr9v(true);
    bsp_gpio_backlight(true);
    pwm_set(PWMCH_FAN, pwm_level_fan_k);
}

void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}
