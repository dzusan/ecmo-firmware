#ifndef __CBUF_H__
#define __CBUF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Buffer descriptor */
struct cbuf
{
    uint8_t* data           ; /* Data storage */
    size_t   item_size     ; /* Single data item size in bytes */
    uint32_t total_items   ; /* Total item number in data storage */
    uint32_t idx_add       ; /* Oldest element index to add value */
    uint32_t idx_read      ; /* Current reading element index */
    uint32_t last_read_flag; /* Last element was read */
};

void cbuf_add(struct cbuf* buf, void* value);
uint8_t cbuf_read(struct cbuf* buf, void* value);

#ifdef __cplusplus
}
#endif
#endif /* __CBUF_H__ */
