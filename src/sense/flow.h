#ifndef SENSE_FLOW_H
#define SENSE_FLOW_H

#include <stdint.h>

enum flow_mode
{
    FLOW_MODE_FLOW,
    FLOW_MODE_COUNT,
};

void flow_init(void);
uint32_t flow_get_uso(void);
uint32_t flow_get_rot(void);
void flow_set_mode(enum flow_mode mode);

#endif /* SENSE_FLOW_H */
