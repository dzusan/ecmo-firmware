#include <assert.h>
#include <errno.h>
#include <stm32f7xx_hal.h>
#include "gpio_def_hal.h"
#include "hall.h"

#include "FreeRTOS.h" // to get configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY only

int hall_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = HALL_U_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(HALL_U_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI2_IRQn,
        configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    GPIO_InitStruct.Pin = HALL_V_Pin | HALL_W_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn,
        configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    return 0;
}

void EXTI2_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(HALL_U_Pin) != RESET)
    {
        hall_irq(hall_read());
        __HAL_GPIO_EXTI_CLEAR_IT(HALL_U_Pin);
    }
}

enum hall_angle hall_read(void)
{
    GPIO_PinState u = HAL_GPIO_ReadPin(HALL_U_GPIO_Port, HALL_U_Pin);
    GPIO_PinState v = HAL_GPIO_ReadPin(HALL_V_GPIO_Port, HALL_V_Pin);
    GPIO_PinState w = HAL_GPIO_ReadPin(HALL_W_GPIO_Port, HALL_W_Pin);
    return u | (v << 1) | (w << 2);
}

void EXTI15_10_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(HALL_V_Pin) != RESET)
    {
        hall_irq(hall_read());
        __HAL_GPIO_EXTI_CLEAR_IT(HALL_V_Pin);
    }
    else if (__HAL_GPIO_EXTI_GET_IT(HALL_W_Pin) != RESET)
    {
        hall_irq(hall_read());
        __HAL_GPIO_EXTI_CLEAR_IT(HALL_W_Pin);
    }
}
