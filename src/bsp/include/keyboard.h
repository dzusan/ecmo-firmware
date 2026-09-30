#ifndef BSP_KEYBOARD_H
#define BSP_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define KEYBOARD_KEY_NUM (7)

/*
1 - GND
2 - KEY7 - PG2
3 - KEY6 - PG3
4 - KEY5 - PG4
5 - KEY4 - PG5
6 - KEY3 - PG6
7 - KEY2 - PG7
8 - KEY1 - PG8
*/

/* ECMO */
/*
#define KEYBOARD_CANCEL (1 << 8)
#define KEYBOARD_UP     (1 << 7)
#define KEYBOARD_MENU   (1 << 6)
#define KEYBOARD_START  (1 << 5)
#define KEYBOARD_LEFT   (1 << 4)
#define KEYBOARD_DOWN   (1 << 3)
#define KEYBOARD_RIGHT  (1 << 2)
*/

/* IRVESO */
#define KEYBOARD_CANCEL (1 << 2)
#define KEYBOARD_UP     (1 << 3)
#define KEYBOARD_MENU   (1 << 4)
#define KEYBOARD_START  (1 << 5)
#define KEYBOARD_LEFT   (1 << 6)
#define KEYBOARD_DOWN   (1 << 7)
#define KEYBOARD_RIGHT  (1 << 8)

int keyboard_init(void);
uint16_t keyboard_read(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_KEYBOARD_H */

