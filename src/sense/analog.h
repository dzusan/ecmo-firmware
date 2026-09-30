#ifndef SENSE_ANALOG_H
#define SENSE_ANALOG_H

#include "adc.h"

#define ANALOG_INVALID_VAL (-32767)

void analog_init(void);
int16_t analog_get(const enum adc_channel id);

#endif /* SENSE_ANALOG_H */
