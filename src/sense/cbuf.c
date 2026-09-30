#include <string.h>
#include "cbuf.h"

void cbuf_add(struct cbuf* buf, void* value)
{
    void* slot = &(buf->data[buf->idx_add * buf->item_size]);
    memcpy(slot, value, (size_t)(buf->item_size));

    if (buf->idx_add < buf->total_items - 1)
    {
        buf->idx_add++;

    }
    else
    {
        buf->idx_add = 0;
    }

    buf->idx_read = buf->idx_add;
    buf->last_read_flag = 0;
}

uint8_t cbuf_read(struct cbuf* buf, void* value)
{
    if (buf->last_read_flag == 1)
    {
        buf->last_read_flag = 0;
        return 0;
    }

    void* slot = &(buf->data[buf->idx_read * buf->item_size]);
    memcpy(value, slot, (size_t)(buf->item_size));

    if (buf->idx_read < buf->total_items - 1)
    {
        buf->idx_read++;
    }
    else
    {
        buf->idx_read = 0;
    }

    if (buf->idx_read == buf->idx_add)
    {
        buf->last_read_flag = 1;
    }

    return 1;
}
