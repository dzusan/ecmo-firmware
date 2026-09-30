#ifndef RUN_TIME_STATS_H
#define RUN_TIME_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

void bsp_high_res_tim_init(void);
uint32_t bsp_high_res_tim_get(void);

#define configUSE_STATS_FORMATTING_FUNCTIONS (1)
#define configGENERATE_RUN_TIME_STATS (1)
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() bsp_high_res_tim_init()
#define portGET_RUN_TIME_COUNTER_VALUE() bsp_high_res_tim_get()

#ifdef __cplusplus
}
#endif
#endif /* RUN_TIME_STATS_H */
