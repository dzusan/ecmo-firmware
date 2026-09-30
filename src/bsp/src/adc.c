#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stm32f7xx_hal.h>
#include "sysutils.h"
#include "adc.h"

#include "FreeRTOS.h" // to get configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY only

static ADC_HandleTypeDef hadc1 = {0};
static ADC_HandleTypeDef hadc3 = {0};

static const struct adc_channel_set
{
    ADC_HandleTypeDef* handler;
    uint32_t channel;
} adc_ch[ADC_CH_TOTAL] =
{
    [ADC_CH_SEN_PRES_1] = {.handler = &hadc3, .channel = ADC_CHANNEL_9}, /* PF3 */
    [ADC_CH_SEN_PRES_2] = {.handler = &hadc3, .channel = ADC_CHANNEL_14}, /* PF4 */

    [ADC_CH_SEN_V_BAT] = {.handler = &hadc3, .channel = ADC_CHANNEL_15}, /* PF5 */
    [ADC_CH_SEN_V_SUP] = {.handler = &hadc3, .channel = ADC_CHANNEL_4}, /* PF6 */

    [ADC_CH_TOUCH_XL] = {.handler = &hadc3, .channel = ADC_CHANNEL_5}, /* PF7 */
    [ADC_CH_TOUCH_YD] = {.handler = &hadc3, .channel = ADC_CHANNEL_6}, /* PF8 */
    [ADC_CH_TOUCH_XR] = {.handler = &hadc3, .channel = ADC_CHANNEL_7}, /* PF9 */
    [ADC_CH_TOUCH_YU] = {.handler = &hadc3, .channel = ADC_CHANNEL_8}, /* PF10 */

    [ADC_CH_SEN_U_I] = {.handler = &hadc1, .channel = ADC_CHANNEL_0}, /* PA0 */
    [ADC_CH_SEN_V_I] = {.handler = &hadc1, .channel = ADC_CHANNEL_7}, /* PA7 */
    [ADC_CH_SEN_V_V] = {.handler = &hadc1, .channel = ADC_CHANNEL_12}, /* PC2 */
    [ADC_CH_SEN_W_V] = {.handler = &hadc1, .channel = ADC_CHANNEL_13}, /* PC3 */
    [ADC_CH_SEN_W_I] = {.handler = &hadc1, .channel = ADC_CHANNEL_14}, /* PC4 */
    [ADC_CH_SEN_U_V] = {.handler = &hadc1, .channel = ADC_CHANNEL_15}, /* PC5 */
};

int adc_init(void)
{
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    memcpy(&hadc3, &hadc1, sizeof(hadc3));

    hadc1.Instance = ADC1;
    hadc3.Instance = ADC3;

    assert(HAL_ADC_Init(&hadc1) == HAL_OK);
    assert(HAL_ADC_Init(&hadc3) == HAL_OK);

    return 0;
}

int adc_start(enum adc_channel id)
{
    if (id >= ADC_CH_TOTAL)
    {
        return -EINVAL;
    }

    ADC_ChannelConfTypeDef ch_conf =
    {
        /* (1 / ((90*(10**6)) / 4)) * 480  = 21 uS */
        .SamplingTime = ADC_SAMPLETIME_480CYCLES,
        .Rank = 1,
        .Channel = adc_ch[id].channel
    };

    HAL_StatusTypeDef s = HAL_ERROR;

    s = HAL_ADC_ConfigChannel(adc_ch[id].handler, &ch_conf);

    if (s != HAL_OK)
    {
        return -EIO;
    }

    s = HAL_ADC_Start_IT(adc_ch[id].handler);

    if (s != HAL_OK)
    {
        return -EIO;
    }

    return 0;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    adc_irq();
}

int adc_get(enum adc_channel id)
{
    if (id >= ADC_CH_TOTAL)
    {
        return -EINVAL;
    }

    return HAL_ADC_GetValue(adc_ch[id].handler);
}

void ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hadc1);
    HAL_ADC_IRQHandler(&hadc3);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (adcHandle->Instance == ADC1)
    {
        /* ADC1 clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC1 GPIO Configuration
        PC2     ------> ADC1_IN12
        PC3     ------> ADC1_IN13
        PA0/WKUP     ------> ADC1_IN0
        PA7     ------> ADC1_IN7
        PC4     ------> ADC1_IN14
        PC5     ------> ADC1_IN15
        */
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC_IRQn,
            configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
    }
    else if (adcHandle->Instance == ADC3)
    {
        /* ADC3 clock enable */
        __HAL_RCC_ADC3_CLK_ENABLE();

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**ADC3 GPIO Configuration
        PF3     ------> ADC3_IN9
        PF4     ------> ADC3_IN14
        PF5     ------> ADC3_IN15
        PF6     ------> ADC3_IN4
        PF7     ------> ADC3_IN5
        PF8     ------> ADC3_IN6
        PF9     ------> ADC3_IN7
        PF10     ------> ADC3_IN8
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
            | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* ADC3 interrupt Init */
        HAL_NVIC_SetPriority(ADC_IRQn,
            configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
    if (adcHandle->Instance == ADC1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /**ADC1 GPIO Configuration
        PC2     ------> ADC1_IN12
        PC3     ------> ADC1_IN13
        PA0/WKUP     ------> ADC1_IN0
        PA7     ------> ADC1_IN7
        PC4     ------> ADC1_IN14
        PC5     ------> ADC1_IN15
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_7);

        /* ADC1 interrupt Deinit */
        /**
        * Uncomment the line below to disable the "ADC_IRQn" interrupt
        * Be aware, disabling shared interrupt may affect other IPs
        */
        /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
    }
    else if (adcHandle->Instance == ADC3)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC3_CLK_DISABLE();

        /**ADC3 GPIO Configuration
        PF3     ------> ADC3_IN9
        PF4     ------> ADC3_IN14
        PF5     ------> ADC3_IN15
        PF6     ------> ADC3_IN4
        PF7     ------> ADC3_IN5
        PF8     ------> ADC3_IN6
        PF9     ------> ADC3_IN7
        PF10     ------> ADC3_IN8
        */
        HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
            | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

        /* ADC3 interrupt Deinit */
        /**
        * Uncomment the line below to disable the "ADC_IRQn" interrupt
        * Be aware, disabling shared interrupt may affect other IPs
        */
        /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
    }
}
