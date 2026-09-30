#ifndef BSP_HALL_H
#define BSP_HALL_H

#ifdef __cplusplus
extern "C" {
#endif

enum hall_angle
{
    HALL_ANGLE_SC  = 0b000, /* short circuit */
    HALL_ANGLE_60  = 0b001,
    HALL_ANGLE_180 = 0b010,
    HALL_ANGLE_120 = 0b011,
    HALL_ANGLE_300 = 0b100,
    HALL_ANGLE_360 = 0b101,
    HALL_ANGLE_240 = 0b110,
    HALL_ANGLE_NC  = 0b111, /* not connected */

    HALL_ANGLE_TOTAL,
};

int hall_init(void);
enum hall_angle hall_read(void);
void hall_irq(enum hall_angle angle); /* Should be defined externally */

#ifdef __cplusplus
}
#endif

#endif /* BSP_HALL_H */
