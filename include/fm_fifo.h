#ifndef __FM_FIFO_H__
#define __FM_FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fmconfig.h>
#include <stdio.h>
#include <stdint.h>

#if defined(FIFO_ENABLE)

typedef struct
{
    uint8_t          *pbuf;
    uint16_t          size;
    volatile uint16_t r_index;
    volatile uint16_t w_index;
} fm_fifo_info_t;

typedef fm_fifo_info_t fm_fifo_instance_t;

#define FM_FIFO_INSTANCE(name, _fifo_size)     \
    static uint8_t   fifo##name[_fifo_size]; \
    fm_fifo_instance_t name = {.pbuf = fifo##name, .size = _fifo_size, .r_index = 0, .w_index = 0};

int fm_fifo_init(fm_fifo_instance_t *pinstance, void *pbuf, uint16_t size);
int fm_fifo_write(fm_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size);
int fm_fifo_read(fm_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size);
int fm_fifo_length(fm_fifo_instance_t *pinstance, uint16_t *plen);
int fm_fifo_flush(fm_fifo_instance_t *pinstance);
int fm_fifo_create(fm_fifo_instance_t *pinstance, uint16_t size);
int fm_fifo_delete(fm_fifo_instance_t *pinstance);

#endif /* FIFO_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __FM_FIFO_H__ */
