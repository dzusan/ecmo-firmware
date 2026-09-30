#include <assert.h>
#include <errno.h>
#include <sys/unistd.h> /* STDOUT_FILENO, STDERR_FILENO */
#include "tty.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"

#define APP_RX_DATA_SIZE       (2048)
#define APP_TX_DATA_SIZE       (2048)

static USBD_HandleTypeDef hUsbDeviceFS;

static uint8_t CDC_Transmit_FS(uint8_t* buf, uint16_t len)
{
    uint8_t result = USBD_OK;
    USBD_CDC_HandleTypeDef* hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;

    if (hcdc->TxState != 0)
    {
        return USBD_BUSY;
    }

    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buf, len);
    result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    return result;
}

static int8_t CDC_Init_FS(void)
{
    static uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
    static uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    return (USBD_OK);
}

static int8_t CDC_DeInit_FS(void)
{
    return (USBD_OK);
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
    return (USBD_OK);
}

static int8_t CDC_Receive_FS(uint8_t* buf, uint32_t* len)
{
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &buf[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);

    /* Echo */
    if (*len > 5)
    {
        CDC_Transmit_FS(buf, *len);
    }

    return (USBD_OK);
}

static int8_t CDC_TransmitCplt_FS(uint8_t* buf, uint32_t* len, uint8_t epnum)
{
    return (USBD_OK);
}

void tty_init(void)
{
    static USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
    {
        CDC_Init_FS,
        CDC_DeInit_FS,
        CDC_Control_FS,
        CDC_Receive_FS,
        CDC_TransmitCplt_FS
    };

    assert(USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) == USBD_OK);
    assert(USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) == USBD_OK);
    assert(USBD_CDC_RegisterInterface(&hUsbDeviceFS,
        &USBD_Interface_fops_FS) == USBD_OK);
    assert(USBD_Start(&hUsbDeviceFS) == USBD_OK);
}

void OTG_FS_IRQHandler(void)
{
    extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

int _write(int file, char* data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }

    uint8_t result = CDC_Transmit_FS((uint8_t*)data, (uint16_t)len);

    if (result != USBD_OK)
    {
        errno = ENXIO;
        return -1;
    }

    return len;
}
