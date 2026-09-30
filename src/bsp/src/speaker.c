#include <assert.h>
#include <errno.h>
#include <stm32f7xx_hal.h>
#include "speaker.h"

static TIM_HandleTypeDef tim_handler;
static const uint32_t k_channel = TIM_CHANNEL_3;
static const uint32_t k_period = 200;

void speaker_init(void)
{
    const uint32_t k_prescaler = 300 - 1;

    __HAL_RCC_TIM3_CLK_ENABLE();

    tim_handler.Instance = TIM3;
    tim_handler.Init.Prescaler = k_prescaler;
    tim_handler.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_handler.Init.Period = k_period - 1;
    tim_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_handler.Init.RepetitionCounter = 0;
    tim_handler.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    assert(HAL_TIM_PWM_Init(&tim_handler) == HAL_OK);

    const TIM_OC_InitTypeDef conf_ch =
    {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = 50,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .OCNPolarity = TIM_OCNPOLARITY_HIGH,
        .OCFastMode = TIM_OCFAST_DISABLE,
        .OCIdleState = TIM_OCIDLESTATE_RESET,
        .OCNIdleState = TIM_OCNIDLESTATE_RESET,
    };

    assert(HAL_TIM_PWM_ConfigChannel(&tim_handler, &conf_ch, k_channel) == HAL_OK);

    /* HAL_TIM_MspPostInit ****************************************************/

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PC8     ------> TIM3_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int speaker_set_freq(uint32_t freq, uint8_t level)
{
    if (level > k_period / 2)
    {
        return -EINVAL;
    }

    if (level == 0 || freq == 0)
    {
        HAL_TIM_PWM_Stop(&tim_handler, k_channel);
    }
    else
    {
        uint32_t presc = 216 * 1000000 / k_period / freq;
        __HAL_TIM_SET_PRESCALER(&tim_handler, presc);
        __HAL_TIM_SET_COMPARE(&tim_handler, k_channel, level);
        HAL_TIM_PWM_Start(&tim_handler, k_channel);
    }

    return 0;
}

int speaker_set_tone(enum speaker_tone tone, uint8_t level)
{
    if (tone >= SPK_TONE_TOTAL || level > k_period / 2)
    {
        return -EINVAL;
    }

    if (level == 0)
    {
        HAL_TIM_PWM_Stop(&tim_handler, k_channel);
    }
    else
    {
        switch (tone)
        {
            case SPK_TONE_HIGH:
                /*(216 * (10**6) / 300) / 255 ~ 2823 Hz */
                __HAL_TIM_SET_PRESCALER(&tim_handler, 300);
                __HAL_TIM_SET_COMPARE(&tim_handler, k_channel, level);
                HAL_TIM_PWM_Start(&tim_handler, k_channel);
                break;

            case SPK_TONE_MIDDLE:
                /*(216 * (10**6) / 500) / 255 ~ 1694 Hz */
                __HAL_TIM_SET_PRESCALER(&tim_handler, 500);
                __HAL_TIM_SET_COMPARE(&tim_handler, k_channel, level);
                HAL_TIM_PWM_Start(&tim_handler, k_channel);
                break;

            case SPK_TONE_LOW:
                /*(216 * (10**6) / 1000) / 255 ~ 847 Hz */
                __HAL_TIM_SET_PRESCALER(&tim_handler, 1000);
                __HAL_TIM_SET_COMPARE(&tim_handler, k_channel, level);
                HAL_TIM_PWM_Start(&tim_handler, k_channel);
                break;

            default:
                return -ENODEV;
        }
    }

    return 0;
}
