#include <assert.h>
#include <errno.h>
#include <stm32f7xx_hal.h>
#include "pwm.h"

static TIM_HandleTypeDef htim1;
static TIM_HandleTypeDef htim4;
static TIM_HandleTypeDef htim12;

static void tim1_init(void)
{
    /* Datasheet (DocID028294 Rev 6), p. 105, Table 14, TIM1 from APB2, 216 MHz
    * (216 * (10**6) / 17) / 255 ~ 49826 Hz
    */
    const uint32_t k_prescaler = 17 - 1;

    __HAL_RCC_TIM1_CLK_ENABLE();

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = k_prescaler;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = pwm_level_max_k;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    assert(HAL_TIM_PWM_Init(&htim1) == HAL_OK);

    const TIM_OC_InitTypeDef conf_ch =
    {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 0,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCNPolarity = TIM_OCNPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE,
        .OCIdleState = TIM_OCIDLESTATE_RESET,
        .OCNIdleState = TIM_OCNIDLESTATE_RESET,
    };

    assert(HAL_TIM_PWM_ConfigChannel(&htim1, &conf_ch, TIM_CHANNEL_1) == HAL_OK);
    assert(HAL_TIM_PWM_ConfigChannel(&htim1, &conf_ch, TIM_CHANNEL_2) == HAL_OK);
    assert(HAL_TIM_PWM_ConfigChannel(&htim1, &conf_ch, TIM_CHANNEL_3) == HAL_OK);

    const TIM_BreakDeadTimeConfigTypeDef conf_dt =
    {
        .OffStateRunMode = TIM_OSSR_DISABLE,
        .OffStateIDLEMode = TIM_OSSI_DISABLE,
        .LockLevel = TIM_LOCKLEVEL_OFF,
        .DeadTime = 100,
        .BreakState = TIM_BREAK_DISABLE,
        .BreakPolarity = TIM_BREAKPOLARITY_LOW,
        .BreakFilter = 0,
        .Break2State = TIM_BREAK2_DISABLE,
        .Break2Polarity = TIM_BREAK2POLARITY_LOW,
        .Break2Filter = 0,
        .AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE,
    };

    assert(HAL_TIMEx_ConfigBreakDeadTime(&htim1, &conf_dt) == HAL_OK);

    /* HAL_TIM_MspPostInit ****************************************************/

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PB13     ------> TIM1_CH1N
    PB14     ------> TIM1_CH2N
    PB15     ------> TIM1_CH3N
    PA8     ------> TIM1_CH1
    PA9     ------> TIM1_CH2
    PA10     ------> TIM1_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void tim4_init(void)
{
    /* Datasheet (DocID028294 Rev 6), p. 106, Table 14, TIM4 from APB1, 108 MHz
     * PULSE(0 3 0 10n 10n 5u 10u)
     * Tperiod = 10u, f = 100 kHz
     * (108 * (10**6) / 4) / 255 ~ 105882 Hz
     */
    const uint32_t k_prescaler = 4 - 1;

    __HAL_RCC_TIM4_CLK_ENABLE();

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = k_prescaler;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = pwm_level_max_k;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.RepetitionCounter = 0;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    assert(HAL_TIM_PWM_Init(&htim4) == HAL_OK);

    const TIM_OC_InitTypeDef conf_ch =
    {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = pwm_middle_k,
        .OCPolarity = TIM_OCPOLARITY_LOW,
        .OCFastMode = TIM_OCFAST_DISABLE,
    };

    assert(HAL_TIM_PWM_ConfigChannel(&htim4, &conf_ch, TIM_CHANNEL_1) == HAL_OK);
    assert(HAL_TIM_PWM_ConfigChannel(&htim4, &conf_ch, TIM_CHANNEL_2) == HAL_OK);
    assert(HAL_TIM_PWM_ConfigChannel(&htim4, &conf_ch, TIM_CHANNEL_3) == HAL_OK);
    assert(HAL_TIM_PWM_ConfigChannel(&htim4, &conf_ch, TIM_CHANNEL_4) == HAL_OK);

    /* HAL_TIM_MspPostInit ****************************************************/

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PD12     ------> TIM4_CH1
    PD13     ------> TIM4_CH2
    PD14     ------> TIM4_CH3
    PD15     ------> TIM4_CH4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void tim12_init(void)
{
    __HAL_RCC_TIM12_CLK_ENABLE();

    htim12.Instance = TIM12;
    htim12.Init.Prescaler = 450;
    htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim12.Init.Period = 4000;
    htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    assert(HAL_TIM_PWM_Init(&htim12) == HAL_OK);

    const TIM_OC_InitTypeDef conf_ch =
    {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 0,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE,
    };

    assert(HAL_TIM_PWM_ConfigChannel(&htim12, &conf_ch, TIM_CHANNEL_1) == HAL_OK);

    /* HAL_TIM_MspPostInit ****************************************************/

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM12 GPIO Configuration
    PB14     ------> TIM12_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM12;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

int pwm_init(void)
{
    __disable_irq();
    tim4_init();
    tim12_init();
    __enable_irq();
    return 0;
}

int pwm_set(enum pwm_channel ch, pwm_level_t level)
{
    if (level > pwm_level_max_k)
    {
        return -EINVAL;
    }

    uint32_t esc_min = 220;
    uint16_t duty = pwm_level_max_k - level;

    switch (ch)
    {
        case PWMCH_MOTOR_U:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
                HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_1);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
                HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_1);
            }

            break;

        case PWMCH_MOTOR_V:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
                HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
                HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_2);
            }

            break;

        case PWMCH_MOTOR_W:
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
                HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
                HAL_TIMEx_OCN_Stop(&htim1, TIM_CHANNEL_3);
            }

            break;

        case PWMCH_EXT_DRV:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, duty);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
            }

            break;

        case PWMCH_FAN:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, duty);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
            }

            break;

        case PWMCH_PUMP_SUP:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, pwm_middle_k);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
            }

            break;

        case PWMCH_PUMP_BAT:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pwm_middle_k);

            if (level > 0)
            {
                HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
            }
            else
            {
                HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
            }

            break;

        case PWMCH_ESC:

            /* 280 - 1.17 ms
             * 380 - 1.59 ms
             */

            if (level > 0)
            {
                __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, esc_min + (uint32_t)level);
            }
            else
            {
                __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, esc_min);
            }

            HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);

            break;

        default:
            return -ENODEV;
    }

    return 0;
}
