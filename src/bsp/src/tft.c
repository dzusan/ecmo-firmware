#include <assert.h>
#include <lvgl.h>
#include "stm32f7xx_hal.h"
#include "gpio_def_hal.h"
#include "tft.h"

#define TFT_VER_RES (360)
#define TFT_HOR_RES (360)

#define  HX8249_WIDTH    ((uint16_t)800)          /* LCD PIXEL WIDTH            */
#define  HX8249_HEIGHT   ((uint16_t)480)          /* LCD PIXEL HEIGHT           */

/****************** HX8249-A01 Parallel RGB at Sync mode **************************/
// DCLK frequency 27.2 MHz
#define  HX8249_HSYNC            ((uint16_t)2)  /* Horizontal synchronization */
#define  HX8249_HBP              ((uint16_t)16) /* Horizontal back porch      */
#define  HX8249_HFP              ((uint16_t)44) /* Horizontal front porch     */
#define  HX8249_VSYNC            ((uint16_t)2)  /* Vertical synchronization   */
#define  HX8249_VBP              ((uint16_t)5)  /* Vertical back porch        */
#define  HX8249_VFP              ((uint16_t)43) /* Vertical front porch       */

static uint8_t framebuf[TFT_VER_RES * TFT_HOR_RES * 2] = {0};
static lv_display_t* disp;

/* LTDC init function */
static void ltdc_init(void)
{
    static LTDC_LayerCfgTypeDef pLayerCfg = {0};
    static LTDC_HandleTypeDef hltdc = {0};

    hltdc.Instance = LTDC;
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    hltdc.Init.HorizontalSync = (HX8249_HSYNC - 1);
    hltdc.Init.VerticalSync = (HX8249_VSYNC - 1);
    hltdc.Init.AccumulatedHBP = (HX8249_HSYNC + HX8249_HBP - 1);
    hltdc.Init.AccumulatedVBP = (HX8249_VSYNC + HX8249_VBP - 1);
    hltdc.Init.AccumulatedActiveH = (HX8249_HEIGHT + HX8249_VSYNC + HX8249_VBP - 1);
    hltdc.Init.AccumulatedActiveW = (HX8249_WIDTH + HX8249_HSYNC + HX8249_HBP - 1);
    hltdc.Init.TotalHeigh = (HX8249_HEIGHT + HX8249_VSYNC + HX8249_VBP + HX8249_VFP
            - 1);
    hltdc.Init.TotalWidth = (HX8249_WIDTH + HX8249_HSYNC + HX8249_HBP + HX8249_HFP -
            1);

    hltdc.Init.Backcolor.Blue = 0;
    hltdc.Init.Backcolor.Green = 0;
    hltdc.Init.Backcolor.Red = 0;

    if (HAL_LTDC_Init(&hltdc) != HAL_OK)
    {
        assert(0);
    }

    pLayerCfg.WindowX0 = (HX8249_WIDTH - TFT_HOR_RES) / 2;
    pLayerCfg.WindowX1 = pLayerCfg.WindowX0 + TFT_HOR_RES;
    pLayerCfg.WindowY0 = (HX8249_HEIGHT - TFT_VER_RES) / 2;;
    pLayerCfg.WindowY1 = pLayerCfg.WindowY0 + TFT_VER_RES;
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    pLayerCfg.Alpha = 150;
    pLayerCfg.Alpha0 = 50;
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    pLayerCfg.FBStartAdress = (uint32_t)&framebuf;
    pLayerCfg.ImageWidth = TFT_HOR_RES;
    pLayerCfg.ImageHeight = TFT_VER_RES;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;

    if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
    {
        assert(0);
    }

    /* Set mode HS+VS */
    HAL_GPIO_WritePin(LTDC_U_MODE_GPIO_Port, LTDC_U_MODE_Pin, GPIO_PIN_SET);

    /* Horizontal shift direction (source output) selection. Set Left to right. */
    HAL_GPIO_WritePin(LTDC_U_LR_GPIO_Port, LTDC_U_LR_Pin, GPIO_PIN_SET);

    /* Vertical shift direction (gate output) selection. Set Top→bottom */
    HAL_GPIO_WritePin(LTDC_U_UD_GPIO_Port, LTDC_U_UD_Pin, GPIO_PIN_SET);

    /* Standby mode OFF */
    HAL_GPIO_WritePin(LTDC_U_DITHB_GPIO_Port, LTDC_U_DITHB_Pin, GPIO_PIN_SET);

    /* Reset up */
    HAL_GPIO_WritePin(LTDC_U_RST_GPIO_Port, LTDC_U_RST_Pin, GPIO_PIN_SET);

}

static void flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map)
{
    lv_display_flush_ready(disp);
}

void tft_init(void)
{
    ltdc_init();
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, framebuf, NULL, sizeof(framebuf),
        LV_DISPLAY_RENDER_MODE_DIRECT);
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* ltdcHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    if (ltdcHandle->Instance == LTDC)
    {
        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
        PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
        PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
        PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
        PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
        PeriphClkInitStruct.PLLSAIDivQ = 1;
        PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;

        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            assert(0);
        }

        /* LTDC clock enable */
        __HAL_RCC_LTDC_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        /**LTDC GPIO Configuration
        PE4     ------> LTDC_B0
        PE5     ------> LTDC_G0
        PE6     ------> LTDC_G1
        PC0     ------> LTDC_R5
        PA1     ------> LTDC_R2
        PA2     ------> LTDC_R1
        PA3     ------> LTDC_B5
        PA4     ------> LTDC_VSYNC
        PA5     ------> LTDC_R4
        PA6     ------> LTDC_G2
        PB0     ------> LTDC_R3
        PB1     ------> LTDC_R6
        PE11     ------> LTDC_G3
        PE12     ------> LTDC_B4
        PE13     ------> LTDC_DE
        PE14     ------> LTDC_CLK
        PE15     ------> LTDC_R7
        PB10     ------> LTDC_G4
        PB11     ------> LTDC_G5
        PD10     ------> LTDC_B3
        PC6     ------> LTDC_HSYNC
        PC7     ------> LTDC_G6
        PC9     ------> LTDC_B2
        PD3     ------> LTDC_G7
        PG12     ------> LTDC_B1
        PG13     ------> LTDC_R0
        PB8     ------> LTDC_B6
        PB9     ------> LTDC_B7
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_11
            | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
            | GPIO_PIN_5 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    }
}

void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* ltdcHandle)
{

    if (ltdcHandle->Instance == LTDC)
    {
        /* Peripheral clock disable */
        __HAL_RCC_LTDC_CLK_DISABLE();

        /**LTDC GPIO Configuration
        PE4     ------> LTDC_B0
        PE5     ------> LTDC_G0
        PE6     ------> LTDC_G1
        PC0     ------> LTDC_R5
        PA1     ------> LTDC_R2
        PA2     ------> LTDC_R1
        PA3     ------> LTDC_B5
        PA4     ------> LTDC_VSYNC
        PA5     ------> LTDC_R4
        PA6     ------> LTDC_G2
        PB0     ------> LTDC_R3
        PB1     ------> LTDC_R6
        PE11     ------> LTDC_G3
        PE12     ------> LTDC_B4
        PE13     ------> LTDC_DE
        PE14     ------> LTDC_CLK
        PE15     ------> LTDC_R7
        PB10     ------> LTDC_G4
        PB11     ------> LTDC_G5
        PD10     ------> LTDC_B3
        PC6     ------> LTDC_HSYNC
        PC7     ------> LTDC_G6
        PC9     ------> LTDC_B2
        PD3     ------> LTDC_G7
        PG12     ------> LTDC_B1
        PG13     ------> LTDC_R0
        PB8     ------> LTDC_B6
        PB9     ------> LTDC_B7
        */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_11
            | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
            | GPIO_PIN_5 | GPIO_PIN_6);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11
            | GPIO_PIN_8 | GPIO_PIN_9);

        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_10 | GPIO_PIN_3);

        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12 | GPIO_PIN_13);
    }
}
