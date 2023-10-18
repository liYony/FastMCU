#include "utils/include/qk_fifo.h"

#if defined(FIFO_ENABLE)

#if defined(MEMH_ENABLE)
#include "utils/include/qk_memh.h"
#endif /* MEMH_ENABLE */

#define FIFO_LEN(w, r, s) (((w) -(r) + (s)) % (s))

int qk_fifo_init(qk_fifo_instance_t *pinstance, void *pbuf, uint16_t size)
{
    if (pinstance == NULL || pbuf == NULL || size == 0)
    {
        return -1;
    }
    pinstance->pbuf    = (uint8_t *)pbuf;
    pinstance->size    = size;
    pinstance->r_index = 0;
    pinstance->w_index = 0;
    return 0;
}

int qk_fifo_length(qk_fifo_instance_t *pinstance, uint16_t *plen)
{
    if (pinstance == NULL || plen == NULL)
    {
        return -1;
    }
    *plen = FIFO_LEN(pinstance->w_index, pinstance->r_index, pinstance->size);
    return 0;
}

int qk_fifo_flush(qk_fifo_instance_t *pinstance)
{
    if (pinstance == NULL)
    {
        return -1;
    }
    pinstance->r_index = pinstance->w_index = 0;
    return 0;
}

int qk_fifo_write(qk_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size)
{
    uint16_t fifo_len, valid_len, index;
    if (pinstance == NULL || pbuf == NULL)
    {
        return -1;
    }
    if (pinstance->pbuf == NULL)
    {
        return -1;
    }
    fifo_len  = FIFO_LEN(pinstance->w_index, pinstance->r_index, pinstance->size);
    valid_len = pinstance->size - fifo_len;
    size      = (size <= valid_len) ? size : valid_len;
    index     = 0;
    while (index < size)
    {
        pinstance->pbuf[pinstance->w_index] = pbuf[index];
        pinstance->w_index                  = (pinstance->w_index + 1) % pinstance->size;
        index += 1;
    }
    return size;
}

int qk_fifo_read(qk_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size)
{
    uint16_t fifo_len, index;
    if (pinstance == NULL || pbuf == NULL)
    {
        return -1;
    }
    if (pinstance->pbuf == NULL)
    {
        return -1;
    }
    fifo_len = FIFO_LEN(pinstance->w_index, pinstance->r_index, pinstance->size);
    size     = (size <= fifo_len) ? size : fifo_len;
    index    = 0;
    while (index < size)
    {
        pbuf[index]        = pinstance->pbuf[pinstance->r_index];
        pinstance->r_index = (pinstance->r_index + 1) % pinstance->size;
        index += 1;
    }
    return size;
}

int qk_fifo_create(qk_fifo_instance_t *pinstance, uint16_t size)
{
#if defined(MEMH_ENABLE)
    if (pinstance == NULL)
    {
        return -1;
    }
    pinstance->pbuf = (uint8_t *)qk_malloc(size);
    if (pinstance->pbuf == NULL)
    {
        return -1;
    }
    pinstance->size    = size;
    pinstance->w_index = 0;
    pinstance->r_index = 0;
    return 0;
#else
    return -1;
#endif /* MEMH_ENABLE */
}

int qk_fifo_delete(qk_fifo_instance_t *pinstance)
{
#if defined(MEMH_ENABLE)
    if (pinstance == NULL)
    {
        return -1;
    }
    if (pinstance->pbuf)
    {
        qk_free(pinstance->pbuf);
        pinstance->pbuf = NULL;
    }
    return 0;
#else
    return -1;
#endif /* MEMH_ENABLE */
}

#endif /* FIFO_ENABLE */
