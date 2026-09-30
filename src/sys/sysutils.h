#ifndef SYSUTILS_H
#define SYSUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ARRAY_SIZE - get the number of elements in array @arr
 * @arr: array to be sized
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))

#ifdef __cplusplus
}
#endif
#endif /* SYSUTILS_H */
