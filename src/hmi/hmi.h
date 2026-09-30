#ifndef HMI_HMI_H
#define HMI_HMI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum hmi_enc_event
{
    HMI_ENC_CW,
    HMI_ENC_CCW,
    HMI_ENC_PRESSED,
    HMI_ENC_RELEASED,
    HMI_ENC_TOTAL,
};

void hmi_init(void);
void hmi_kb_cb(uint16_t key, bool state);
void hmi_enc_cb(enum hmi_enc_event ev);

#ifdef __cplusplus
}
#endif

#endif /* HMI_HMI_H */

