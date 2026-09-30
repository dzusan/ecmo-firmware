#ifndef BSP_SPEAKER_H
#define BSP_SPEAKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum speaker_tone
{
    SPK_TONE_LOW,
    SPK_TONE_MIDDLE,
    SPK_TONE_HIGH,
    SPK_TONE_TOTAL,
};

void speaker_init(void);
int speaker_set_freq(uint32_t freq, uint8_t level);
int speaker_set_tone(enum speaker_tone tone, uint8_t level);

#ifdef __cplusplus
}
#endif

#endif /* BSP_SPEAKER_H */

