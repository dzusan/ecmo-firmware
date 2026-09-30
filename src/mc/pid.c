#include <stdlib.h>
#include "pwm.h"
#include "pid.h"

struct pid_conf_fixed
{
    /* PID coefficients in fixed point
     * format DEC 2.3 (factor 0.001)
     * 0.001 units
     */

    uint32_t Kp;
    uint16_t Ki;
    uint16_t Kd;
    uint16_t int_win_th;
} pid_coefs =
{
    .Kp         = 300,   /* 0.3  */
    .Ki         = 500,   /* 0.5  */
    .Kd         = 0,     /* 0.0  */
    .int_win_th = 10000, /* 10.0 */
};

pwm_level_t pid_proc(int32_t preset, int32_t fact, int32_t max, double* I,
    double* last_err)
{
    /* Externally set PID coefficients */
    double Kp = (double)pid_coefs.Kp / 1000.0;
    double Ki = (double)pid_coefs.Ki / 1000.0;
    double Kd = (double)pid_coefs.Kd / 1000.0;
    double int_win_th = (double)pid_coefs.int_win_th / 1000.0;

    /* Current system state varriables */
    double error = (double)(preset - fact) / max;
    double P = error;
    double D = 0.0;
    double C = 0.0;
    pwm_level_t pwm = pwm_min_k;

    if (preset <= max)
    {
        /* Integration */
        *I += error * ((double)pid_period_ms_k * 0.001); /* milliseconds */

        /* Windup guarding */
        if (*I < -int_win_th)
        {
            *I = -int_win_th;
        }
        else if (*I > int_win_th)
        {
            *I = int_win_th;
        }

        /* Differentiation */
        D = (error - *last_err) / ((double)pid_period_ms_k * 0.001); /* milliseconds */
        *last_err = error;

        /* Result control value */
        C = (Kp * P) + (Ki * (*I)) + (Kd * D);

        /* PWM calculation */
        if (C > 0)
        {
            pwm = (pwm_level_t)(C * (pwm_max_k - pwm_min_k)) + pwm_min_k;

            /* Saturation */
            if (pwm > pwm_max_k)
            {
                return pwm_max_k;
            }
            else if (pwm < pwm_min_k)
            {
                return pwm_min_k;
            }
        }

        return pwm;
    }
    else
    {
        return 0;
    }
}
