#include <fm_fifo.h>

#if defined(FIFO_ENABLE)

#if defined(MEM_ENABLE)
#include <fm_mem.h>
#endif /* MEM_ENABLE */

#define FIFO_LEN(w, r, s) (((w) -(r) + (s)) % (s))

int fm_fifo_init(fm_fifo_instance_t *pinstance, void *pbuf, uint16_t size)
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

int fm_fifo_length(fm_fifo_instance_t *pinstance, uint16_t *plen)
{
    if (pinstance == NULL || plen == NULL)
    {
        return -1;
    }
    *plen = FIFO_LEN(pinstance->w_index, pinstance->r_index, pinstance->size);
    return 0;
}

int fm_fifo_flush(fm_fifo_instance_t *pinstance)
{
    if (pinstance == NULL)
    {
        return -1;
    }
    pinstance->r_index = pinstance->w_index = 0;
    return 0;
}

int fm_fifo_write(fm_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size)
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

int fm_fifo_read(fm_fifo_instance_t *pinstance, uint8_t *pbuf, uint16_t size)
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

int fm_fifo_create(fm_fifo_instance_t *pinstance, uint16_t size)
{
#if defined(MEM_ENABLE)
    if (pinstance == NULL)
    {
        return -1;
    }
    pinstance->pbuf = (uint8_t *)fm_malloc(size);
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
#endif /* MEM_ENABLE */
}

int fm_fifo_delete(fm_fifo_instance_t *pinstance)
{
#if defined(MEM_ENABLE)
    if (pinstance == NULL)
    {
        return -1;
    }
    if (pinstance->pbuf)
    {
        fm_free(pinstance->pbuf);
        pinstance->pbuf = NULL;
    }
    return 0;
#else
    return -1;
#endif /* MEM_ENABLE */
}

#endif /* FIFO_ENABLE */
