#include <assert.h>
#include <errno.h>
#include <stm32f7xx_hal.h>
#include "gpio_def_hal.h"
#include "digin.h"

#include "FreeRTOS.h" // to get configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY only

#define FLOW_ROT_Port GPIOD
#define FLOW_ROT_Pin  GPIO_PIN_5

#define ESC_FB_Port GPIOA
#define ESC_FB_Pin  GPIO_PIN_9

int digin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SEN_FLOW_F_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SEN_FLOW_F_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn,
        configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    GPIO_InitStruct.Pin = SEN_FLOW_R_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SEN_FLOW_R_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI1_IRQn,
        configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    GPIO_InitStruct.Pin = ESC_FB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ESC_FB_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn,
        configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    GPIO_InitStruct.Pin = SEN_FLOW_C_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SEN_FLOW_C_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ENC_A_Pin | ENC_B_Pin | ENC_BTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENC_A_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = FLOW_ROT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(FLOW_ROT_Port, &GPIO_InitStruct);

    return 0;
}

int digin_get_state(enum digin_id id)
{
    switch (id)
    {
        case DIGIN_FLOW_F:
            return (int)HAL_GPIO_ReadPin(SEN_FLOW_F_GPIO_Port, SEN_FLOW_F_Pin);

        case DIGIN_FLOW_R:
            return (int)HAL_GPIO_ReadPin(SEN_FLOW_R_GPIO_Port, SEN_FLOW_R_Pin);

        case DIGIN_FLOW_C:
            return (int)HAL_GPIO_ReadPin(SEN_FLOW_C_GPIO_Port, SEN_FLOW_C_Pin);

        case DIGIN_FLOW_ROT:
            return (int)HAL_GPIO_ReadPin(FLOW_ROT_Port, FLOW_ROT_Pin);

        case DIGIN_ENC_A:
            return (int)HAL_GPIO_ReadPin(ENC_A_GPIO_Port, ENC_A_Pin);

        case DIGIN_ENC_B:
            return (int)HAL_GPIO_ReadPin(ENC_B_GPIO_Port, ENC_B_Pin);

        case DIGIN_ENC_BTN:
            return (int)HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin);

        default:
            break;
    }

    return -EINVAL;
}

int digin_proc_front(enum digin_id id)
{
    if (id < DIGIN_FLOW_TOTAL)
    {
        enum stage
        {
            IDLE,
            FALL,
        };

        static enum stage fsm[DIGIN_FLOW_TOTAL] = {0};
        int cur = digin_get_state(id);

        if (cur == 0 && fsm[id] == IDLE)
        {
            fsm[id] = FALL;
            return 1;
        }
        else if (cur != 0 && fsm[id] == FALL)
        {
            fsm[id] = IDLE;
        }
    }

    return 0;
}

void EXTI0_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(SEN_FLOW_F_Pin) != RESET)
    {
        digin_irq_flow_f();
        __HAL_GPIO_EXTI_CLEAR_IT(SEN_FLOW_F_Pin);
    }
}

void EXTI1_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(SEN_FLOW_R_Pin) != RESET)
    {
        digin_irq_flow_r();
        __HAL_GPIO_EXTI_CLEAR_IT(SEN_FLOW_R_Pin);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(ESC_FB_Pin) != RESET)
    {
        digin_irq_esc_fb();
        __HAL_GPIO_EXTI_CLEAR_IT(ESC_FB_Pin);
    }
}
