#ifndef MC_INIT_H
#define MC_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

int mc_init(void);
void mc_enable(bool en);
int32_t mc_get_speed(void);
int32_t mc_get_max(void);
int32_t mc_set_speed(int32_t speed);

#ifdef __cplusplus
}
#endif

#endif /* MC_INIT_H */

