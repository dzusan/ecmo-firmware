set(USB_PATH ${CMAKE_CURRENT_LIST_DIR})

set(USB_C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/cubegen/usbd_desc.c
    ${CMAKE_CURRENT_LIST_DIR}/cubegen/usbd_conf.c
    ${CMAKE_CURRENT_LIST_DIR}/tty/mon.c
    ${CMAKE_CURRENT_LIST_DIR}/tty/tty.c
    ${THIRD_PARTY_ROOT}/stm32_mw_usb_device/Core/Src/usbd_ctlreq.c
    ${THIRD_PARTY_ROOT}/stm32_mw_usb_device/Class/CDC/Src/usbd_cdc.c
    ${THIRD_PARTY_ROOT}/stm32f7xx_hal_driver/Src/stm32f7xx_ll_usb.c
    ${THIRD_PARTY_ROOT}/stm32_mw_usb_device/Core/Src/usbd_ioreq.c
    ${THIRD_PARTY_ROOT}/stm32_mw_usb_device/Core/Src/usbd_core.c
)
    
set(USB_INCLUDES 
    ${CMAKE_CURRENT_LIST_DIR}/cubegen
    ${CMAKE_CURRENT_LIST_DIR}/tty
    ${THIRD_PARTY_ROOT}/stm32_mw_usb_device/Core/Inc
    ${THIRD_PARTY_ROOT}/stm32_mw_usb_device/Class/CDC/Inc
)
