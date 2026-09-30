#ifndef SENSE_TEMPERATURE_H
#define SENSE_TEMPERATURE_H

#include <stdint.h>

#define TEMPERATURE_INVALID_VAL (-100.0)

enum temperature_ch
{
    TEMPERATURE_CH_MOT,
    TEMPERATURE_CH_ENV,
    TEMPERATURE_CH_ETC,
    TEMPERATURE_CH_TOTAL,
};

void temperature_init(void);
double temperature_get(enum temperature_ch ch);

#endif /* SENSE_TEMPERATURE_H */
