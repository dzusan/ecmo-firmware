#ifndef MON_H
#define MON_H

#ifdef __cplusplus
extern "C" {
#endif

enum mon_id
{
    MON_KB,
    MON_RPM,
    MON_MC_PWM,
    MON_P1,
    MON_P2,
    MON_PD,
    MON_FLOW_USO,
    MON_FLOW_ROT,
    MON_U,
    MON_MC_I,
    MON_MC_P,
    MON_BTN,
    MON_MC_STAGE,
    MON_MC_ROT_CNT,

    MON_TM,
    MON_TO,

    MON_TOTAL
};

int mon_init(void);
int mon_write(enum mon_id, int value);
void mon_trig(void);

#ifdef __cplusplus
}
#endif

#endif /* MON_H */

