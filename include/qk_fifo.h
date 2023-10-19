#ifndef __QK_FIFO_H__
#define __QK_FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <qkconfig.h>
#include <stdio.h>
#include <stdint.h>

#if defined(FIFO_ENABLE)

typedef struct
{
    uint8_t          *pbuf;
    uint16_t          size;
    volatile uint16_t r_index;
    volatile uint16_t w_index;
} qk_fifo_info_t;

typedef qk_fifo_info_t qk_fifo_instance_t;

#define QK_FIFO_INSTANCE(name, _fifo_size)     \
    static uint8_t   fifo##name[_fifo_size]; \
    qk_fifo_instance_t name = {.pbuf = fifo##name, .size = _fifo_size, .r_index = 0, .w_index = 0};

int qk_fifo_init(qk_fifo_instance_t *pinstance, void *pbuf, uint16_t size);
int qk_fifo_write(qk_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size);
int qk_fifo_read(qk_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size);
int qk_fifo_length(qk_fifo_instance_t *pinstance, uint16_t *plen);
int qk_fifo_flush(qk_fifo_instance_t *pinstance);
int qk_fifo_create(qk_fifo_instance_t *pinstance, uint16_t size);
int qk_fifo_delete(qk_fifo_instance_t *pinstance);

#endif /* FIFO_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __QK_FIFO_H__ */
