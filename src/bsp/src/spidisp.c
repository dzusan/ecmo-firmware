/*
Reference
https://docs.lvgl.io/master/integration/driver/display/lcd_stm32_guide.html#lcd-stm32-guide
*/

#include <assert.h>
#include <lvgl.h>
#include "stm32f7xx_hal.h"
#include "gpio_def_hal.h"
#include "spidisp.h"

#include "FreeRTOS.h" // to get configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY only
#include "./src/drivers/display/st7796/lv_st7796.h"

#define LCD_H_RES       320
#define LCD_V_RES       480
#define BUS_SPI1_POLL_TIMEOUT 0x1000U

SPI_HandleTypeDef spi_handle;
DMA_HandleTypeDef dma_handle;
lv_display_t* lcd_disp;

static void periph_init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(
        DMA1_Stream5_IRQn,
        configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY,
        0);

    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    /* Sitronix ST7796S (2014.11, p.53)
     * TSCYCR   Serial clock cycle (Read)   150 ns   6  Mbit/s   16
     * TSCYCW   Serial clock cycle (Write)  66  ns   15 Mbit/s   4
     * works flawlessly                              27 Mbit/s   2 */
    spi_handle.Instance               = SPI3                      ;
    spi_handle.Init.Mode              = SPI_MODE_MASTER           ;
    spi_handle.Init.Direction         = SPI_DIRECTION_2LINES      ;
    spi_handle.Init.DataSize          = SPI_DATASIZE_8BIT         ;
    spi_handle.Init.CLKPolarity       = SPI_POLARITY_LOW          ;
    spi_handle.Init.CLKPhase          = SPI_PHASE_1EDGE           ;
    spi_handle.Init.NSS               = SPI_NSS_SOFT              ;
    spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2   ;
    spi_handle.Init.FirstBit          = SPI_FIRSTBIT_MSB          ;
    spi_handle.Init.TIMode            = SPI_TIMODE_DISABLE        ;
    spi_handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    spi_handle.Init.CRCPolynomial     = 0                         ;
    spi_handle.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE   ;
    spi_handle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE     ;

    assert(HAL_SPI_Init(&spi_handle) == HAL_OK);

    /* LED ON */
    HAL_GPIO_WritePin(DISP_SD_CS_GPIO_Port, DISP_SD_CS_Pin, GPIO_PIN_SET);

    /* Reset up */
    HAL_GPIO_WritePin(DISP_RST_GPIO_Port, DISP_RST_Pin, GPIO_PIN_SET);

    /* Chip select up */
    HAL_GPIO_WritePin(DISP_LCD_CS_GPIO_Port, DISP_LCD_CS_Pin, GPIO_PIN_SET);
}

static void set_datasize_bit(SPI_HandleTypeDef* hspi, uint8_t bitlen)
{
    uint32_t mask = SPI_DATASIZE_8BIT ^ SPI_DATASIZE_16BIT;

    if (bitlen == 16)
    {
        hspi->Init.DataSize = SPI_DATASIZE_16BIT;
        SET_BIT(hspi->Instance->CR2, mask);
        CLEAR_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD_QF);
    }
    else
    {
        hspi->Init.DataSize = SPI_DATASIZE_8BIT;
        CLEAR_BIT(hspi->Instance->CR2, mask);
        SET_BIT(hspi->Instance->CR2, SPI_RXFIFO_THRESHOLD_QF);
    }
}

/* Platform-specific implementation of the LCD send command function. In general this should use polling transfer. */
static void lcd_send_cmd(lv_display_t* disp, const uint8_t* cmd,
    size_t cmd_size, const uint8_t* param, size_t param_size)
{
    (void)disp;

    /* Set the SPI in 8-bit mode */
    set_datasize_bit(&spi_handle, 8);
    /* DCX low (command) */
    HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_RESET);
    /* CS low */
    HAL_GPIO_WritePin(DISP_LCD_CS_GPIO_Port, DISP_LCD_CS_Pin, GPIO_PIN_RESET);

    /* send command */
    if (HAL_SPI_Transmit(&spi_handle, (uint8_t*)cmd, cmd_size,
        BUS_SPI1_POLL_TIMEOUT) == HAL_OK)
    {
        /* DCX high (data) */
        HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_SET);
        /* for short data blocks we use polling transfer */
        HAL_SPI_Transmit(&spi_handle, (uint8_t*)param, (uint16_t)param_size,
            BUS_SPI1_POLL_TIMEOUT);
        /* CS high */
        HAL_GPIO_WritePin(DISP_LCD_CS_GPIO_Port, DISP_LCD_CS_Pin, GPIO_PIN_SET);
    }
}

/* Platform-specific implementation of the LCD send color function. For better performance this should use DMA transfer.
 * In case of a DMA transfer a callback must be installed to notify LVGL about the end of the transfer.
 */
static void lcd_send_color(lv_display_t* disp, const uint8_t* cmd,
    size_t cmd_size, uint8_t* param, size_t param_size)
{
    LV_UNUSED(disp);

    /* Set the SPI in 8-bit mode */
    set_datasize_bit(&spi_handle, 8);
    /* DCX low (command) */
    HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_RESET);
    /* CS low */
    HAL_GPIO_WritePin(DISP_LCD_CS_GPIO_Port, DISP_LCD_CS_Pin, GPIO_PIN_RESET);

    /* send command */
    if (HAL_SPI_Transmit(&spi_handle, (uint8_t*)cmd, cmd_size,
        BUS_SPI1_POLL_TIMEOUT) == HAL_OK)
    {
        /* DCX high (data) */
        HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_SET);
        /* for color data use DMA transfer */
        /* Set the SPI in 16-bit mode to match endianess */
        set_datasize_bit(&spi_handle, 16);
        HAL_SPI_Transmit_DMA(&spi_handle, param, (uint16_t)param_size / 2);
        /* NOTE: CS will be reset in the transfer ready callback */
    }
}

void spidisp_init(void)
{
    periph_init();

    /* Create the LVGL display object and the LCD display driver */
    lcd_disp = lv_st7796_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_BGR,
            lcd_send_cmd, lcd_send_color);
    lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_270);

    static uint8_t buf1[LCD_H_RES * LCD_V_RES * 3 / 8];
    static uint8_t buf2[LCD_H_RES * LCD_V_RES * 3 / 8];

    lv_display_set_buffers(
        lcd_disp,
        buf1,
        buf2,
        sizeof(buf1),
        LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    if (hspi == &spi_handle)
    {
        HAL_GPIO_WritePin(DISP_LCD_CS_GPIO_Port, DISP_LCD_CS_Pin, GPIO_PIN_SET);
        lv_display_flush_ready(lcd_disp);
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    if (hspi == &spi_handle)
    {
        __HAL_RCC_SPI3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /*
        SPI3 GPIO Configuration
        PB3     ------> SPI3_SCK
        PB4     ------> SPI3_MISO
        PB5     ------> SPI3_MOSI
        */

        GPIO_InitTypeDef desc =
        {
            .Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5,
            .Mode = GPIO_MODE_AF_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_VERY_HIGH,
            .Alternate = GPIO_AF6_SPI3,
        };

        HAL_GPIO_Init(GPIOB, &desc);

        dma_handle.Instance = DMA1_Stream5;
        dma_handle.Init.Channel = DMA_CHANNEL_0;
        dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
        dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
        dma_handle.Init.MemInc = DMA_MINC_ENABLE;
        dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        dma_handle.Init.Mode = DMA_NORMAL;
        dma_handle.Init.Priority = DMA_PRIORITY_LOW;
        dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        assert(HAL_DMA_Init(&dma_handle) == HAL_OK);
        __HAL_LINKDMA(hspi, hdmatx, dma_handle);
    }
}

void DMA1_Stream5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&dma_handle);
}
