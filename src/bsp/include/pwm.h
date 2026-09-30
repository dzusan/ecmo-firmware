#ifndef BSP_PWM_H
#define BSP_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum pwm_channel
{
    PWMCH_MOTOR_U,
    PWMCH_MOTOR_V,
    PWMCH_MOTOR_W,

    PWMCH_EXT_DRV,  /* PD12, TIM4_CH1 */
    PWMCH_FAN,      /* PD13, TIM4_CH2 */
    PWMCH_PUMP_SUP, /* PD14, TIM4_CH3 */
    PWMCH_PUMP_BAT, /* PD15, TIM4_CH4 */

    PWMCH_ESC,      /* PD14, TIM12_CH1 */

    PWMCH_TOTAL,
};

typedef uint8_t pwm_level_t;
static const pwm_level_t pwm_level_max_k = 0xFF;
static const pwm_level_t pwm_middle_k = pwm_level_max_k / 2;
static const pwm_level_t pwm_level_fan_k = 225;

int pwm_init(void);
int pwm_set(enum pwm_channel ch, pwm_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* BSP_PWM_H */
