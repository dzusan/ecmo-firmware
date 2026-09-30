#ifndef MC_PID_H
#define MC_PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pwm.h"

static const uint32_t pid_period_ms_k = 600;
static const pwm_level_t pwm_min_k = 125;
static const pwm_level_t pwm_max_k = 250;

pwm_level_t pid_proc(int32_t preset, int32_t fact, int32_t max, double* I,
    double* last_err);

#ifdef __cplusplus
}
#endif

#endif /* MC_PID_H */
