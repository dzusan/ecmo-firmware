#include <assert.h>
#include <stm32f7xx_hal.h>
#include "gpio_def_hal.h"
#include "keyboard.h"

#define KEYBOARD_MASK (\
    KEYBOARD_CANCEL |\
    KEYBOARD_UP |\
    KEYBOARD_MENU |\
    KEYBOARD_START |\
    KEYBOARD_LEFT |\
    KEYBOARD_DOWN |\
    KEYBOARD_RIGHT)

int keyboard_init(void)
{
    GPIO_InitTypeDef init =
    {
        .Pin = KEY7_Pin | KEY6_Pin | KEY5_Pin | KEY4_Pin
        | KEY3_Pin | KEY2_Pin | KEY1_Pin,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP
    };

    HAL_GPIO_Init(GPIOG, &init);
    return 0;
}

uint16_t keyboard_read(void)
{
    return GPIOG->IDR & KEYBOARD_MASK;
}
