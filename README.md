# ecmo-firmware

An extracorporeal membrane oxygenation (ECMO) control system for a portable pump prototype with a brushless motor driver, pressure, flow and temperature sensors, and an LVGL-based HMI.

Clone repo with submodules! 

`git clone --recurse-submodules git@github.com:inventa-lab/ecmo-firmware.git`


# Required tools

* make, gcc (for linux only)
* CMake (>= 3.18)
* [astyle](https://astyle.sourceforge.net/)


# Dependencies

* [freertos](https://github.com/FreeRTOS/FreeRTOS-Kernel)
* [stm32f7xx_hal_driver](https://github.com/STMicroelectronics/stm32f7xx_hal_driver)
* [cmsis_device_f7](https://github.com/STMicroelectronics/cmsis_device_f7)
* [cmsis_core](https://github.com/STMicroelectronics/cmsis_core)
* [lvgl](https://github.com/lvgl/lvgl)
* [stm32_mw_usb_device](https://github.com/STMicroelectronics/stm32_mw_usb_device)


# Build

1. `cmake . -B bin`
2. `cmake --build bin` (use `-j16` to faster building)


# Code style

`astyle --options=codestyle.txt -r "src/*.c" "src/*.h"`


# References

* [STM32 HAL stock](https://github.com/STMicroelectronics/STM32Cube_MCU_Overall_Offer)
