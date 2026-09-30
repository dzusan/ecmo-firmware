#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void bsp_gpio_init(void);
void bsp_gpio_led(bool state);
void bsp_gpio_pwr9v(bool state);
void bsp_gpio_backlight(bool state);
void bsp_gpio_bkp_sup(bool state);
void bsp_gpio_bkp_bat(bool state);

#ifdef __cplusplus
}
#endif
#endif /* BSP_GPIO_H */