#include <assert.h>
#include <FreeRTOS.h>
#include <task.h>
#include <lvgl.h>
#include "bsp_init.h"
#include "bsp_gpio.h"
#include "hmi.h"
#include "mc_init.h"
#include "mon.h"
#include "tty.h"
#include "analog.h"
#include "flow.h"
#include "battery.h"
#include "temperature.h"

int main(void)
{
    lv_init();
    bsp_init();
    tty_init();
    mon_init();

    mc_init();
    // battery_init();

    hmi_init();
    analog_init();
    flow_init();
    temperature_init();
    vTaskStartScheduler();
    assert("FreeRTOS not started");
}
