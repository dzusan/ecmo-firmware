#ifndef BSP_DIGIN_H
#define BSP_DIGIN_H

#ifdef __cplusplus
extern "C" {
#endif

enum digin_id
{
    DIGIN_FLOW_F,
    DIGIN_FLOW_R,
    DIGIN_FLOW_C,
    DIGIN_FLOW_ROT,
    DIGIN_ENC_A,
    DIGIN_ENC_B,
    DIGIN_ENC_BTN,
    DIGIN_FLOW_TOTAL,
};

int digin_init(void);
int digin_get_state(enum digin_id id);
int digin_proc_front(enum digin_id id);
void digin_irq_flow_f(void); /* Should be defined externally */
void digin_irq_flow_r(void); /* Should be defined externally */
void digin_irq_esc_fb(void); /* Should be defined externally */

#ifdef __cplusplus
}
#endif

#endif /* BSP_DIGIN_H */
