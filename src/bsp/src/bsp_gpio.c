#include "gpio_def_hal.h"
#include "bsp_gpio.h"

void bsp_gpio_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, DBG1_Pin | DBG2_Pin | LTDC_U_MODE_Pin | GATE_BAT_Pin
        | GATE_SUP_Pin | LTDC_U_RST_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, PWR_9V_EN_Pin | PS_ON_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOG, LTDC_U_LR_Pin | LTDC_U_UD_Pin | DISP_DC_Pin,
        GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD,
        LTDC_U_DITHB_Pin | PWM_BACKLIGHT_Pin | LED_Pin | DISP_SD_CS_Pin,
        GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, DISP_LCD_CS_Pin | DISP_RST_Pin, GPIO_PIN_RESET);

    bsp_gpio_bkp_sup(false);
    bsp_gpio_bkp_bat(false);

    /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                             PEPin PEPin */
    GPIO_InitStruct.Pin = DBG1_Pin | DBG2_Pin | LTDC_U_MODE_Pin | GATE_BAT_Pin
        | GATE_SUP_Pin | LTDC_U_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pins : PCPin PCPin */
    GPIO_InitStruct.Pin = PWR_9V_EN_Pin | PS_ON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = PWR_GOOD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(PWR_GOOD_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : PFPin PFPin PFPin PFPin
                             PFPin PFPin PFPin PFPin */
    GPIO_InitStruct.Pin = SEN_FLOW_F_Pin | SEN_FLOW_R_Pin | SEN_FLOW_C_Pin |
        HALL_V_Pin
        | HALL_W_Pin | ENC_A_Pin | ENC_B_Pin | ENC_BTN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = HALL_U_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(HALL_U_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : PGPin PGPin PGPin */
    GPIO_InitStruct.Pin = LTDC_U_LR_Pin | LTDC_U_UD_Pin | DISP_DC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /*Configure GPIO pins : PDPin PDPin PDPin PDPin */
    GPIO_InitStruct.Pin = LTDC_U_DITHB_Pin | PWM_BACKLIGHT_Pin | LED_Pin |
        DISP_SD_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : PGPin PGPin PGPin PGPin
                             PGPin PGPin PGPin */
    GPIO_InitStruct.Pin = KEY7_Pin | KEY6_Pin | KEY5_Pin | KEY4_Pin
        | KEY3_Pin | KEY2_Pin | KEY1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /*Configure GPIO pins : PBPin PBPin */
    GPIO_InitStruct.Pin = DISP_LCD_CS_Pin | DISP_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void bsp_gpio_led(bool state)
{
    if (state)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    }
}

void bsp_gpio_pwr9v(bool state)
{
    if (state)
    {
        HAL_GPIO_WritePin(PWR_9V_EN_GPIO_Port, PWR_9V_EN_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(PWR_9V_EN_GPIO_Port, PWR_9V_EN_Pin, GPIO_PIN_RESET);
    }
}

void bsp_gpio_backlight(bool state)
{
    if (state)
    {
        HAL_GPIO_WritePin(PWM_BACKLIGHT_GPIO_Port, PWM_BACKLIGHT_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(PWM_BACKLIGHT_GPIO_Port, PWM_BACKLIGHT_Pin, GPIO_PIN_RESET);
    }
}

void bsp_gpio_bkp_sup(bool state)
{
    /* Inverted pin */

    if (state)
    {
        HAL_GPIO_WritePin(GATE_SUP_GPIO_Port, GATE_SUP_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GATE_SUP_GPIO_Port, GATE_SUP_Pin, GPIO_PIN_SET);
    }
}

void bsp_gpio_bkp_bat(bool state)
{
    /* Inverted pin */

    if (state)
    {
        HAL_GPIO_WritePin(GATE_BAT_GPIO_Port, GATE_BAT_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GATE_BAT_GPIO_Port, GATE_BAT_Pin, GPIO_PIN_SET);
    }
}
