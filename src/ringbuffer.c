#include <fmcu.h>

fm_inline enum fm_ringbuffer_state fm_ringbuffer_status(struct fm_ringbuffer *rb)
{
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
            return FM_RINGBUFFER_EMPTY;
        else
            return FM_RINGBUFFER_FULL;
    }
    return FM_RINGBUFFER_HALFFULL;
}

/**
 * @brief Initialize the ring buffer object.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param pool      A pointer to the buffer.
 * @param size      The size of the buffer in bytes.
 */
void fm_ringbuffer_init(struct fm_ringbuffer *rb,
                        fm_uint8_t           *pool,
                        fm_int32_t            size)
{
    FM_ASSERT(rb != FM_NULL);
    FM_ASSERT(size > 0);

    /* initialize read and write index */
    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = FM_ALIGN_DOWN(size, FM_ALIGN_SIZE);
}

/**
 * @brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will discard out-of-range data.
 *
 * @param rb            A pointer to the ring buffer object.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of data in bytes.
 *
 * @return Return the data size we put into the ring buffer.
 */
fm_size_t fm_ringbuffer_put(struct fm_ringbuffer *rb,
                            const fm_uint8_t     *ptr,
                            fm_uint32_t           length)
{
    fm_uint32_t size;

    FM_ASSERT(rb != FM_NULL);

    /* whether has enough space */
    size = fm_ringbuffer_space_len(rb);

    /* no space */
    if (size == 0)
        return 0;

    /* drop some data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        fm_memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

    fm_memcpy(&rb->buffer_ptr[rb->write_index],
              &ptr[0],
              rb->buffer_size - rb->write_index);
    fm_memcpy(&rb->buffer_ptr[0],
              &ptr[rb->buffer_size - rb->write_index],
              length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

/**
 * @brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will overwrite the existing data in the ring buffer.
 *
 * @param rb            A pointer to the ring buffer object.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of data in bytes.
 *
 * @return Return the data size we put into the ring buffer.
 */
fm_size_t fm_ringbuffer_put_force(struct fm_ringbuffer *rb,
                                  const fm_uint8_t     *ptr,
                                  fm_uint32_t           length)
{
    fm_uint32_t space_length;

    FM_ASSERT(rb != FM_NULL);

    space_length = fm_ringbuffer_space_len(rb);

    if (length > rb->buffer_size)
    {
        ptr = &ptr[length - rb->buffer_size];
        length = rb->buffer_size;
    }

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        fm_memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;

        if (length > space_length)
            rb->read_index = rb->write_index;

        return length;
    }

    fm_memcpy(&rb->buffer_ptr[rb->write_index],
              &ptr[0],
              rb->buffer_size - rb->write_index);
    fm_memcpy(&rb->buffer_ptr[0],
              &ptr[rb->buffer_size - rb->write_index],
              length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    if (length > space_length)
    {
        if (rb->write_index <= rb->read_index)
            rb->read_mirror = ~rb->read_mirror;
        rb->read_index = rb->write_index;
    }

    return length;
}

/**
 * @brief Get data from the ring buffer.
 *
 * @param rb            A pointer to the ring buffer.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of the data we want to read from the ring buffer.
 *
 * @return Return the data size we read from the ring buffer.
 */
fm_size_t fm_ringbuffer_get(struct fm_ringbuffer *rb,
                            fm_uint8_t           *ptr,
                            fm_uint32_t           length)
{
    fm_size_t size;

    FM_ASSERT(rb != FM_NULL);

    /* whether has enough data  */
    size = fm_ringbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if (size < length)
        length = size;

    if (rb->buffer_size - rb->read_index > length)
    {
        /* copy all of data */
        fm_memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    fm_memcpy(&ptr[0],
              &rb->buffer_ptr[rb->read_index],
              rb->buffer_size - rb->read_index);
    fm_memcpy(&ptr[rb->buffer_size - rb->read_index],
              &rb->buffer_ptr[0],
              length - (rb->buffer_size - rb->read_index));

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

/**
 * @brief Get the first readable byte of the ring buffer.
 *
 * @param rb        A pointer to the ringbuffer.
 * @param ptr       When this function return, *ptr is a pointer to the first readable byte of the ring buffer.
 *
 * @note It is recommended to read only one byte, otherwise it may cause buffer overflow.
 *
 * @return Return the size of the ring buffer.
 */
fm_size_t fm_ringbuffer_peek(struct fm_ringbuffer *rb, fm_uint8_t **ptr)
{
    fm_size_t size;

    FM_ASSERT(rb != FM_NULL);

    *ptr = FM_NULL;

    /* whether has enough data  */
    size = fm_ringbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    *ptr = &rb->buffer_ptr[rb->read_index];

    if ((fm_size_t)(rb->buffer_size - rb->read_index) > size)
    {
        rb->read_index += size;
        return size;
    }

    size = rb->buffer_size - rb->read_index;

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = 0;

    return size;
}

/**
 * @brief Put a byte into the ring buffer. If ring buffer is full, this operation will fail.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param ch        A byte put into the ring buffer.
 *
 * @return Return the data size we put into the ring buffer. The ring buffer is full if returns 0. Otherwise, it will return 1.
 */
fm_size_t fm_ringbuffer_putchar(struct fm_ringbuffer *rb, const fm_uint8_t ch)
{
    FM_ASSERT(rb != FM_NULL);

    /* whether has enough space */
    if (!fm_ringbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    }
    else
    {
        rb->write_index++;
    }

    return 1;
}

/**
 * @brief Put a byte into the ring buffer. If ring buffer is full, it will discard an old data and put into a new data.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param ch        A byte put into the ring buffer.
 *
 * @return Return the data size we put into the ring buffer. Always return 1.
 */
fm_size_t fm_ringbuffer_putchar_force(struct fm_ringbuffer *rb, const fm_uint8_t ch)
{
    enum fm_ringbuffer_state old_state;

    FM_ASSERT(rb != FM_NULL);

    old_state = fm_ringbuffer_status(rb);

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
        if (old_state == FM_RINGBUFFER_FULL)
        {
            rb->read_mirror = ~rb->read_mirror;
            rb->read_index = rb->write_index;
        }
    }
    else
    {
        rb->write_index++;
        if (old_state == FM_RINGBUFFER_FULL)
            rb->read_index = rb->write_index;
    }

    return 1;
}

/**
 * @brief Get a byte from the ring buffer.
 *
 * @param rb        The pointer to the ring buffer object.
 * @param ch        A pointer to the buffer, used to store one byte.
 *
 * @return 0    The ring buffer is empty.
 * @return 1    Success
 */
fm_size_t fm_ringbuffer_getchar(struct fm_ringbuffer *rb, fm_uint8_t *ch)
{
    FM_ASSERT(rb != FM_NULL);

    /* ringbuffer is empty */
    if (!fm_ringbuffer_data_len(rb))
        return 0;

    /* put byte */
    *ch = rb->buffer_ptr[rb->read_index];

    if (rb->read_index == rb->buffer_size - 1)
    {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = 0;
    }
    else
    {
        rb->read_index++;
    }

    return 1;
}

/**
 * @brief Get the size of data in the ring buffer in bytes.
 *
 * @param rb        The pointer to the ring buffer object.
 *
 * @return Return the size of data in the ring buffer in bytes.
 */
fm_size_t fm_ringbuffer_data_len(struct fm_ringbuffer *rb)
{
    switch (fm_ringbuffer_status(rb))
    {
    case FM_RINGBUFFER_EMPTY:
        return 0;
    case FM_RINGBUFFER_FULL:
        return rb->buffer_size;
    case FM_RINGBUFFER_HALFFULL:
    default:
    {
        fm_size_t wi = rb->write_index, ri = rb->read_index;

        if (wi > ri)
            return wi - ri;
        else
            return rb->buffer_size - (ri - wi);
    }
    }
}

/**
 * @brief Reset the ring buffer object, and clear all contents in the buffer.
 *
 * @param rb        A pointer to the ring buffer object.
 */
void fm_ringbuffer_reset(struct fm_ringbuffer *rb)
{
    FM_ASSERT(rb != FM_NULL);

    rb->read_mirror = 0;
    rb->read_index = 0;
    rb->write_mirror = 0;
    rb->write_index = 0;
}

#ifdef FM_USING_HEAP
/**
 * @brief Create a ring buffer object with a given size.
 *
 * @param size      The size of the buffer in bytes.
 *
 * @return Return a pointer to ring buffer object. When the return value is FM_NULL, it means this creation failed.
 */
struct fm_ringbuffer *fm_ringbuffer_create(fm_uint32_t size)
{
    struct fm_ringbuffer *rb;
    fm_uint8_t *pool;

    FM_ASSERT(size > 0);

    size = FM_ALIGN_DOWN(size, FM_ALIGN_SIZE);

    rb = (struct fm_ringbuffer *)fm_malloc(sizeof(struct fm_ringbuffer));
    if (rb == FM_NULL)
        goto exit;

    pool = (fm_uint8_t *)fm_malloc(size);
    if (pool == FM_NULL)
    {
        fm_free(rb);
        rb = FM_NULL;
        goto exit;
    }
    fm_ringbuffer_init(rb, pool, size);

exit:
    return rb;
}

/**
 * @brief Destroy the ring buffer object, which is created by fm_ringbuffer_create() .
 *
 * @param rb        A pointer to the ring buffer object.
 */
void fm_ringbuffer_destroy(struct fm_ringbuffer *rb)
{
    FM_ASSERT(rb != FM_NULL);

    fm_free(rb->buffer_ptr);
    fm_free(rb);
}
#endif /* FM_USING_HEAP */