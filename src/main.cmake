include(src/sys/sys.cmake)
include(src/freertos_config/freertos_config.cmake)
include(src/hmi/hmi.cmake)
include(src/mc/mc.cmake)
include(src/usb/usb.cmake)
include(src/sense/sense.cmake)
include(src/battery/battery.cmake)

set(LINKER_FILE ${CMAKE_CURRENT_LIST_DIR}/app.ld)

set(CONFIG_C_SOURCES
    ${SYS_C_SOURCES}
    ${FREERTOS_CONFIG_C_SOURCES}
    ${HMI_C_SOURCES}
    ${MC_C_SOURCES}
    ${USB_C_SOURCES}
    ${SENSE_C_SOURCES}
    ${BATTERY_C_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/main.c 
)
    
set(CONFIG_INCLUDES 
    ${BSP_PATH}
    ${SYS_INCLUDES}
    ${FREERTOS_CONFIG_INCLUDES}
    ${HMI_INCLUDES}
    ${MC_INCLUDES}
    ${USB_INCLUDES}
    ${SENSE_INCLUDES}
    ${BATTERY_INCLUDES}
)
