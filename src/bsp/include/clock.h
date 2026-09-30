#ifndef __CLOCK_H__
#define __CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

void clock_rcc_init(void);
void bsp_high_res_tim_init(void);
uint32_t bsp_high_res_tim_get(void);

#ifdef __cplusplus
}
#endif
#endif /* __CLOCK_H__ */