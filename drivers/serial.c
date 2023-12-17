#include <fmdevice.h>
#include <fmcu.h>

#define DBG_TAG    "Serial"
#define DBG_LVL    DBG_INFO
#include <fmdbg.h>

static fm_ssize_t fm_serial_get_linear_buffer(struct fm_ringbuffer       *rb,
                                                    fm_uint8_t         **ptr)
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

    if(rb->buffer_size - rb->read_index > size)
    {
        return size;
    }

    return rb->buffer_size - rb->read_index;
}

static fm_ssize_t fm_serial_update_read_index(struct fm_ringbuffer    *rb,
                                                    fm_uint16_t       read_index)
{
    fm_size_t size;

    FM_ASSERT(rb != FM_NULL);

    /* whether has enough data  */
    size = fm_ringbuffer_data_len(rb);

    /* no data */
    if (size == 0)
        return 0;

    /* less data */
    if(size < read_index)
        read_index = size;

    if(rb->buffer_size - rb->read_index > read_index)
    {
        rb->read_index += read_index;
        return read_index;
    }

    read_index = rb->buffer_size - rb->read_index;

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = 0;

    return read_index;
}

static fm_ssize_t fm_serial_update_write_index(struct fm_ringbuffer  *rb,
                                                     fm_uint16_t     write_size)
{
    fm_uint16_t size;
    FM_ASSERT(rb != FM_NULL);

    /* whether has enough space */
    size = fm_ringbuffer_space_len(rb);

    /* no space, drop some data */
    if (size < write_size)
    {
        write_size = size;
        LOG_W("The serial buffer (len %d) is overflow.", rb->buffer_size);
    }

    if (rb->buffer_size - rb->write_index > write_size)
    {
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += write_size;
        return write_size;
    }

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = write_size - (rb->buffer_size - rb->write_index);

    return write_size;
}


/**
  * @brief Serial polling receive data routine, This function will receive data
  *        in a continuous loop by one by one byte.
  * @param dev The pointer of device driver structure
  * @param pos Empty parameter.
  * @param buffer Receive data buffer.
  * @param size Receive data buffer length.
  * @return Return the final length of data received.
  */
fm_ssize_t _serial_poll_rx(struct fm_device          *dev,
                                 fm_off_t            pos,
                                 void               *buffer,
                                 fm_size_t           size)
{
    struct fm_serial_device *serial;
    fm_size_t getc_size;
    int getc_element;      /* Gets one byte of data received */
    fm_uint8_t *getc_buffer;     /* Pointer to the receive data buffer */

    FM_ASSERT(dev != FM_NULL);

    serial = (struct fm_serial_device *)dev;
    FM_ASSERT(serial != FM_NULL);
    getc_buffer = (fm_uint8_t *)buffer;
    getc_size = size;

    while(size)
    {
        getc_element = serial->ops->getc(serial);
        if (getc_element == -1) break;

        *getc_buffer = getc_element;

        ++ getc_buffer;
        -- size;

        if (serial->parent.open_flag & FM_DEVICE_FLAG_STREAM)
        {
            /* If open_flag satisfies FM_DEVICE_FLAG_STREAM
             * and the received character is '\n', exit the loop directly */
            if (getc_element == '\n') break;
        }
    }

   return getc_size - size;
}

/**
  * @brief Serial polling transmit data routines, This function will transmit
  *        data in a continuous loop by one by one byte.
  * @param dev The pointer of device driver structure
  * @param pos Empty parameter.
  * @param buffer Transmit data buffer.
  * @param size Transmit data buffer length.
  * @return Return the final length of data received.
  */
fm_ssize_t _serial_poll_tx(struct fm_device           *dev,
                                 fm_off_t             pos,
                                 const void          *buffer,
                                 fm_size_t            size)
{
    struct fm_serial_device *serial;
    fm_size_t putc_size;
    fm_uint8_t *putc_buffer;    /* Pointer to the transmit data buffer */
    FM_ASSERT(dev != FM_NULL);

    serial = (struct fm_serial_device *)dev;
    FM_ASSERT(serial != FM_NULL);

    putc_buffer = (fm_uint8_t *)buffer;
    putc_size = size;

    while (size)
    {
        if (serial->parent.open_flag & FM_DEVICE_FLAG_STREAM)
        {
            /* If open_flag satisfies FM_DEVICE_FLAG_STREAM and the received character is '\n',
             * inserts '\r' character before '\n' character for the effect of carriage return newline */
            if (*putc_buffer == '\n')
                serial->ops->putc(serial, '\r');
        }
        serial->ops->putc(serial, *putc_buffer);

        ++ putc_buffer;
        -- size;
    }

     return putc_size - size;
}

/**
  * @brief Serial receive data routines, This function will receive
  *        data by using fifo
  * @param dev The pointer of device driver structure
  * @param pos Empty parameter.
  * @param buffer Receive data buffer.
  * @param size Receive data buffer length.
  * @return Return the final length of data received.
  */
static fm_ssize_t _serial_fifo_rx(struct fm_device        *dev,
                                        fm_off_t          pos,
                                        void             *buffer,
                                        fm_size_t         size)
{
    struct fm_serial_device *serial;
    struct fm_serial_rx_fifo *rx_fifo;
    fm_base_t level;
    fm_size_t recv_len;  /* The length of data from the ringbuffer */

    FM_ASSERT(dev != FM_NULL);
    if (size == 0) return 0;

    serial = (struct fm_serial_device *)dev;

    FM_ASSERT((serial != FM_NULL) && (buffer != FM_NULL));

    rx_fifo = (struct fm_serial_rx_fifo *) serial->serial_rx;

    if (dev->open_flag & FM_SERIAL_RX_BLOCKING)
    {
        if (size > serial->config.rx_bufsz)
        {
            LOG_W("(%s) serial device received data:[%d] larger than "
               "rx_bufsz:[%d], please increase the BSP_UARTx_RX_BUFSIZE option",
                dev->name, size, serial->config.rx_bufsz);

            return 0;
        }
        /* Get the length of the data from the ringbuffer */
        recv_len = fm_ringbuffer_data_len(&(rx_fifo->rb));

        if (recv_len < size)
        {
            /* When recv_len is less than size, rx_cpt_index is updated to the size
            * and fm_current_thread is suspend until rx_cpt_index is equal to 0 */
            rx_fifo->rx_cpt_index = size;
            fm_completion_wait(&(rx_fifo->rx_cpt), FM_WAITING_FOREVER);
        }
    }

    /* This part of the code is open_flag as FM_SERIAL_RX_NON_BLOCKING */

    level = fm_hw_interrupt_disable();
    /* When open_flag is FM_SERIAL_RX_NON_BLOCKING,
     * the data is retrieved directly from the ringbuffer and returned */
    recv_len = fm_ringbuffer_get(&(rx_fifo->rb), buffer, size);

    fm_hw_interrupt_enable(level);

    return recv_len;
}

/**
  * @brief Serial transmit data routines, This function will transmit
  *        data by using blocking_nbuf.
  * @param dev The pointer of device driver structure
  * @param pos Empty parameter.
  * @param buffer Transmit data buffer.
  * @param size Transmit data buffer length.
  * @return Return the final length of data transmit.
  */
static fm_ssize_t _serial_fifo_tx_blocking_nbuf(struct fm_device        *dev,
                                                      fm_off_t          pos,
                                                const void             *buffer,
                                                      fm_size_t         size)
{
    struct fm_serial_device *serial;
    struct fm_serial_tx_fifo *tx_fifo = FM_NULL;
    fm_ssize_t rst;

    FM_ASSERT(dev != FM_NULL);
    if (size == 0) return 0;

    serial = (struct fm_serial_device *)dev;
    FM_ASSERT((serial != FM_NULL) && (buffer != FM_NULL));
    tx_fifo = (struct fm_serial_tx_fifo *) serial->serial_tx;
    FM_ASSERT(tx_fifo != FM_NULL);

    if (serial->parent.open_flag & FM_DEVICE_FLAG_STREAM)
    {
        return _serial_poll_tx(dev, pos, buffer, size);
    }

    /* When serial transmit in tx_blocking mode,
     * if the activated mode is FM_TRUE, it will return directly */
    if (tx_fifo->activated == FM_TRUE)  return 0;

    tx_fifo->activated = FM_TRUE;
    /* Call the transmit interface for transmission */
    rst = serial->ops->transmit(serial,
                                (fm_uint8_t *)buffer,
                                size,
                                FM_SERIAL_TX_BLOCKING);
    /* Waiting for the transmission to complete */
    fm_completion_wait(&(tx_fifo->tx_cpt), FM_WAITING_FOREVER);

    return rst;
}

/**
  * @brief Serial transmit data routines, This function will transmit
  *        data by using blocking_buf.
  * @param dev The pointer of device driver structure
  * @param pos Empty parameter.
  * @param buffer Transmit data buffer.
  * @param size Transmit data buffer length.
  * @return Return the final length of data transmit.
  */
static fm_ssize_t _serial_fifo_tx_blocking_buf(struct fm_device        *dev,
                                                     fm_off_t          pos,
                                               const void             *buffer,
                                                    fm_size_t          size)
{
    struct fm_serial_device *serial;
    struct fm_serial_tx_fifo *tx_fifo = FM_NULL;
    fm_size_t length = size;
    fm_size_t offset = 0;

    if (size == 0) return 0;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;
    FM_ASSERT((serial != FM_NULL) && (buffer != FM_NULL));

    tx_fifo = (struct fm_serial_tx_fifo *) serial->serial_tx;
    FM_ASSERT(tx_fifo != FM_NULL);

    if (serial->parent.open_flag & FM_DEVICE_FLAG_STREAM)
    {
        return _serial_poll_tx(dev, pos, buffer, size);
    }
    /* When serial transmit in tx_blocking mode,
     * if the activated mode is FM_TRUE, it will return directly */
    if (tx_fifo->activated == FM_TRUE)  return 0;
    tx_fifo->activated = FM_TRUE;

    while (size)
    {
        /* Copy one piece of data into the ringbuffer at a time
         * until the length of the data is equal to size */
        tx_fifo->put_size = fm_ringbuffer_put(&(tx_fifo->rb),
                                               (fm_uint8_t *)buffer + offset,
                                               size);

        /* Call the transmit interface for transmission */
        serial->ops->transmit(serial,
                             (fm_uint8_t *)buffer + offset,
                             tx_fifo->put_size,
                             FM_SERIAL_TX_BLOCKING);

        offset += tx_fifo->put_size;
        size -= tx_fifo->put_size;
        /* Waiting for the transmission to complete */
        fm_completion_wait(&(tx_fifo->tx_cpt), FM_WAITING_FOREVER);
    }
    /* Finally Inactivate the tx->fifo */
    tx_fifo->activated = FM_FALSE;

    return length;
}

/**
  * @brief Serial transmit data routines, This function will transmit
  *        data by using nonblocking.
  * @param dev The pointer of device driver structure
  * @param pos Empty parameter.
  * @param buffer Transmit data buffer.
  * @param size Transmit data buffer length.
  * @return Return the final length of data transmit.
  */
static fm_ssize_t _serial_fifo_tx_nonblocking(struct fm_device        *dev,
                                                    fm_off_t          pos,
                                              const void             *buffer,
                                                    fm_size_t         size)
{
    struct fm_serial_device *serial;
    struct fm_serial_tx_fifo *tx_fifo;
    fm_base_t level;
    fm_size_t length;

    FM_ASSERT(dev != FM_NULL);
    if (size == 0) return 0;

    serial = (struct fm_serial_device *)dev;
    FM_ASSERT((serial != FM_NULL) && (buffer != FM_NULL));
    tx_fifo = (struct fm_serial_tx_fifo *) serial->serial_tx;

    level = fm_hw_interrupt_disable();

    if (tx_fifo->activated == FM_FALSE)
    {
        /* When serial transmit in tx_non_blocking mode, if the activated mode is FM_FALSE,
         * start copying data into the ringbuffer */
        tx_fifo->activated = FM_TRUE;
        /* Copying data into the ringbuffer */
        length = fm_ringbuffer_put(&(tx_fifo->rb), buffer, size);

        fm_hw_interrupt_enable(level);

        fm_uint8_t *put_ptr = FM_NULL;
        /* Get the linear length buffer from rinbuffer */
        tx_fifo->put_size = fm_serial_get_linear_buffer(&(tx_fifo->rb), &put_ptr);
        /* Call the transmit interface for transmission */
        serial->ops->transmit(serial,
                              put_ptr,
                              tx_fifo->put_size,
                              FM_SERIAL_TX_NON_BLOCKING);
        /* In tx_nonblocking mode, there is no need to call fm_completion_wait() APIs to wait
         * for the fm_current_thread to resume */
        return length;
    }

    /* If the activated mode is FM_TRUE, it means that serial device is transmitting,
     * where only the data in the ringbuffer and there is no need to call the transmit() API.
     * Note that this part of the code requires disable interrupts
     * to prevent multi thread reentrant */

    /* Copying data into the ringbuffer */
    length = fm_ringbuffer_put(&(tx_fifo->rb), buffer, size);

    fm_hw_interrupt_enable(level);

    return length;
}


/**
  * @brief Enable serial transmit mode.
  * @param dev The pointer of device driver structure
  * @param rx_oflag The flag of that the serial port opens.
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_tx_enable(struct fm_device        *dev,
                                           fm_uint16_t       tx_oflag)
{
    struct fm_serial_device *serial;
    struct fm_serial_tx_fifo *tx_fifo = FM_NULL;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

    if (serial->config.tx_bufsz == 0)
    {
        /* Cannot use FM_SERIAL_TX_NON_BLOCKING when tx_bufsz is 0 */
        if (tx_oflag == FM_SERIAL_TX_NON_BLOCKING)
        {
            LOG_E("(%s) serial device with misconfigure: tx_bufsz = 0",
                    dev->name);
            return -FM_EINVAL;
        }

#ifndef FM_USING_DEVICE_OPS
        dev->write = _serial_poll_tx;
#endif

        dev->open_flag |= FM_SERIAL_TX_BLOCKING;
        return FM_EOK;
    }
    /* Limits the minimum value of tx_bufsz */
    if (serial->config.tx_bufsz < FM_SERIAL_TX_MINBUFSZ)
        serial->config.tx_bufsz = FM_SERIAL_TX_MINBUFSZ;

    if (tx_oflag == FM_SERIAL_TX_BLOCKING)
    {
        /* When using FM_SERIAL_TX_BLOCKING, it is necessary to determine
         * whether serial device needs to use buffer */
        fm_err_t optmode;  /* The operating mode used by serial device */
        /* Call the Control() API to get the operating mode */
        optmode = serial->ops->control(serial,
                                       FM_DEVICE_CHECK_OPTMODE,
                                       (void *)FM_DEVICE_FLAG_TX_BLOCKING);
        if (optmode == FM_SERIAL_TX_BLOCKING_BUFFER)
        {
            /* If use FM_SERIAL_TX_BLOCKING_BUFFER, the ringbuffer is initialized */
            tx_fifo = (struct fm_serial_tx_fifo *) fm_malloc
                    (sizeof(struct fm_serial_tx_fifo) + serial->config.tx_bufsz);
            FM_ASSERT(tx_fifo != FM_NULL);

            fm_ringbuffer_init(&(tx_fifo->rb),
                                tx_fifo->buffer,
                                serial->config.tx_bufsz);
            serial->serial_tx = tx_fifo;

#ifndef FM_USING_DEVICE_OPS
            dev->write = _serial_fifo_tx_blocking_buf;
#endif
        }
        else
        {
            /* If not use FM_SERIAL_TX_BLOCKING_BUFFER,
             * the control() API is called to configure the serial device */
            tx_fifo = (struct fm_serial_tx_fifo*) fm_malloc
                    (sizeof(struct fm_serial_tx_fifo));
            FM_ASSERT(tx_fifo != FM_NULL);

            serial->serial_tx = tx_fifo;

#ifndef FM_USING_DEVICE_OPS
            dev->write = _serial_fifo_tx_blocking_nbuf;
#endif

            /* Call the control() API to configure the serial device by FM_SERIAL_TX_BLOCKING*/
            serial->ops->control(serial,
                                FM_DEVICE_CTRL_CONFIG,
                                (void *)FM_SERIAL_TX_BLOCKING);
            fm_memset(&tx_fifo->rb, FM_NULL, sizeof(tx_fifo->rb));
        }

        tx_fifo->activated = FM_FALSE;
        tx_fifo->put_size = 0;
        fm_completion_init(&(tx_fifo->tx_cpt));
        dev->open_flag |= FM_SERIAL_TX_BLOCKING;

        return FM_EOK;
    }
    /* When using FM_SERIAL_TX_NON_BLOCKING, ringbuffer needs to be initialized,
     * and initialize the tx_fifo->activated value is FM_FALSE.
     */
    tx_fifo = (struct fm_serial_tx_fifo *) fm_malloc
            (sizeof(struct fm_serial_tx_fifo) + serial->config.tx_bufsz);
    FM_ASSERT(tx_fifo != FM_NULL);

    tx_fifo->activated = FM_FALSE;
    tx_fifo->put_size = 0;
    fm_ringbuffer_init(&(tx_fifo->rb),
                        tx_fifo->buffer,
                        serial->config.tx_bufsz);
    serial->serial_tx = tx_fifo;

#ifndef FM_USING_DEVICE_OPS
    dev->write = _serial_fifo_tx_nonblocking;
#endif

    dev->open_flag |= FM_SERIAL_TX_NON_BLOCKING;
    /* Call the control() API to configure the serial device by FM_SERIAL_TX_NON_BLOCKING*/
    serial->ops->control(serial,
                        FM_DEVICE_CTRL_CONFIG,
                        (void *)FM_SERIAL_TX_NON_BLOCKING);

    return FM_EOK;
}


/**
  * @brief Enable serial receive mode.
  * @param dev The pointer of device driver structure
  * @param rx_oflag The flag of that the serial port opens.
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_rx_enable(struct fm_device        *dev,
                                           fm_uint16_t       rx_oflag)
{
    struct fm_serial_device *serial;
    struct fm_serial_rx_fifo *rx_fifo = FM_NULL;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

    if (serial->config.rx_bufsz == 0)
    {
        /* Cannot use FM_SERIAL_RX_NON_BLOCKING when rx_bufsz is 0 */
        if (rx_oflag == FM_SERIAL_RX_NON_BLOCKING)
        {
            LOG_E("(%s) serial device with misconfigure: rx_bufsz = 0",
                    dev->name);
            return -FM_EINVAL;
        }

#ifndef FM_USING_DEVICE_OPS
        dev->read = _serial_poll_rx;
#endif

        dev->open_flag |= FM_SERIAL_RX_BLOCKING;
        return FM_EOK;
    }
    /* Limits the minimum value of rx_bufsz */
    if (serial->config.rx_bufsz < FM_SERIAL_RX_MINBUFSZ)
        serial->config.rx_bufsz = FM_SERIAL_RX_MINBUFSZ;

    rx_fifo = (struct fm_serial_rx_fifo *) fm_malloc
            (sizeof(struct fm_serial_rx_fifo) + serial->config.rx_bufsz);

    FM_ASSERT(rx_fifo != FM_NULL);
    fm_ringbuffer_init(&(rx_fifo->rb), rx_fifo->buffer, serial->config.rx_bufsz);

    serial->serial_rx = rx_fifo;

#ifndef FM_USING_DEVICE_OPS
    dev->read = _serial_fifo_rx;
#endif

    if (rx_oflag == FM_SERIAL_RX_NON_BLOCKING)
    {
        dev->open_flag |= FM_SERIAL_RX_NON_BLOCKING;
        /* Call the control() API to configure the serial device by FM_SERIAL_RX_NON_BLOCKING*/
        serial->ops->control(serial,
                            FM_DEVICE_CTRL_CONFIG,
                            (void *) FM_SERIAL_RX_NON_BLOCKING);

        return FM_EOK;
    }
    /* When using FM_SERIAL_RX_BLOCKING, fm_completion_init() and rx_cpt_index are initialized */
    rx_fifo->rx_cpt_index = 0;
    fm_completion_init(&(rx_fifo->rx_cpt));
    dev->open_flag |= FM_SERIAL_RX_BLOCKING;
    /* Call the control() API to configure the serial device by FM_SERIAL_RX_BLOCKING*/
    serial->ops->control(serial,
                        FM_DEVICE_CTRL_CONFIG,
                        (void *) FM_SERIAL_RX_BLOCKING);

    return FM_EOK;
}

/**
  * @brief Disable serial receive mode.
  * @param dev The pointer of device driver structure
  * @param rx_oflag The flag of that the serial port opens.
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_rx_disable(struct fm_device        *dev,
                                            fm_uint16_t       rx_oflag)
{
    struct fm_serial_device *serial;
    struct fm_serial_rx_fifo *rx_fifo;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

#ifndef FM_USING_DEVICE_OPS
    dev->read = FM_NULL;
#endif

    if (serial->serial_rx == FM_NULL) return FM_EOK;

    do
    {
        if (rx_oflag == FM_SERIAL_RX_NON_BLOCKING)
        {
            dev->open_flag &= ~ FM_SERIAL_RX_NON_BLOCKING;
            serial->ops->control(serial,
                                FM_DEVICE_CTRL_CLR_INT,
                                (void *)FM_SERIAL_RX_NON_BLOCKING);
            break;
        }

        dev->open_flag &= ~ FM_SERIAL_RX_BLOCKING;
        serial->ops->control(serial,
                            FM_DEVICE_CTRL_CLR_INT,
                            (void *)FM_SERIAL_RX_BLOCKING);
    } while (0);

    rx_fifo = (struct fm_serial_rx_fifo *)serial->serial_rx;
    FM_ASSERT(rx_fifo != FM_NULL);
    fm_free(rx_fifo);
    serial->serial_rx = FM_NULL;

    return FM_EOK;
}

/**
  * @brief Disable serial tranmit mode.
  * @param dev The pointer of device driver structure
  * @param rx_oflag The flag of that the serial port opens.
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_tx_disable(struct fm_device        *dev,
                                            fm_uint16_t       tx_oflag)
{
    struct fm_serial_device *serial;
    struct fm_serial_tx_fifo *tx_fifo;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

#ifndef FM_USING_DEVICE_OPS
    dev->write = FM_NULL;
#endif

    if (serial->serial_tx == FM_NULL) return FM_EOK;

    tx_fifo = (struct fm_serial_tx_fifo *)serial->serial_tx;
    FM_ASSERT(tx_fifo != FM_NULL);

    do
    {
        if (tx_oflag == FM_SERIAL_TX_NON_BLOCKING)
        {
            dev->open_flag &= ~ FM_SERIAL_TX_NON_BLOCKING;

            serial->ops->control(serial,
                                FM_DEVICE_CTRL_CLR_INT,
                                (void *)FM_SERIAL_TX_NON_BLOCKING);
            break;
        }

        fm_completion_done(&(tx_fifo->tx_cpt));
        dev->open_flag &= ~ FM_SERIAL_TX_BLOCKING;
        serial->ops->control(serial,
                            FM_DEVICE_CTRL_CLR_INT,
                            (void *)FM_SERIAL_TX_BLOCKING);
    } while (0);

    fm_free(tx_fifo);
    serial->serial_tx = FM_NULL;

    return FM_EOK;
}

/**
  * @brief Initialize the serial device.
  * @param dev The pointer of device driver structure
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_init(struct fm_device *dev)
{
    fm_err_t result = FM_EOK;
    struct fm_serial_device *serial;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;
    FM_ASSERT(serial->ops->transmit != FM_NULL);

    /* initialize rx/tx */
    serial->serial_rx = FM_NULL;
    serial->serial_tx = FM_NULL;

    /* apply configuration */
    if (serial->ops->configure)
        result = serial->ops->configure(serial, &serial->config);

    return result;
}

/**
  * @brief Open the serial device.
  * @param dev The pointer of device driver structure
  * @param oflag The flag of that the serial port opens.
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_open(struct fm_device *dev, fm_uint16_t oflag)
{
    struct fm_serial_device *serial;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

    /* Check that the device has been turned on */
    if ((dev->open_flag) & (15 << 12))
    {
        LOG_D("(%s) serial device has already been opened, it will run in its original configuration", dev->name);
        return FM_EOK;
    }

    LOG_D("open serial device: 0x%08x with open flag: 0x%04x",
        dev, oflag);

    /* By default, the receive mode of a serial devide is FM_SERIAL_RX_NON_BLOCKING */
    if ((oflag & FM_SERIAL_RX_BLOCKING) == FM_SERIAL_RX_BLOCKING)
        dev->open_flag |= FM_SERIAL_RX_BLOCKING;
    else
        dev->open_flag |= FM_SERIAL_RX_NON_BLOCKING;

    /* By default, the transmit mode of a serial devide is FM_SERIAL_TX_BLOCKING */
    if ((oflag & FM_SERIAL_TX_NON_BLOCKING) == FM_SERIAL_TX_NON_BLOCKING)
        dev->open_flag |= FM_SERIAL_TX_NON_BLOCKING;
    else
        dev->open_flag |= FM_SERIAL_TX_BLOCKING;

    /* set steam flag */
    if ((oflag & FM_DEVICE_FLAG_STREAM) ||
        (dev->open_flag & FM_DEVICE_FLAG_STREAM))
        dev->open_flag |= FM_DEVICE_FLAG_STREAM;

    /* initialize the Rx structure according to open flag */
    if (serial->serial_rx == FM_NULL)
        fm_serial_rx_enable(dev, dev->open_flag &
                            (FM_SERIAL_RX_BLOCKING | FM_SERIAL_RX_NON_BLOCKING));

    /* initialize the Tx structure according to open flag */
    if (serial->serial_tx == FM_NULL)
        fm_serial_tx_enable(dev, dev->open_flag &
                            (FM_SERIAL_TX_BLOCKING | FM_SERIAL_TX_NON_BLOCKING));

    return FM_EOK;
}


/**
  * @brief Close the serial device.
  * @param dev The pointer of device driver structure
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_close(struct fm_device *dev)
{
    struct fm_serial_device *serial;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

    /* this device has more reference count */
    if (dev->ref_count > 1) return -FM_ERROR;
    /* Disable serial receive mode. */
    fm_serial_rx_disable(dev, dev->open_flag &
                        (FM_SERIAL_RX_BLOCKING | FM_SERIAL_RX_NON_BLOCKING));
    /* Disable serial tranmit mode. */
    fm_serial_tx_disable(dev, dev->open_flag &
                        (FM_SERIAL_TX_BLOCKING | FM_SERIAL_TX_NON_BLOCKING));

    /* Clear the callback function */
    serial->parent.rx_indicate = FM_NULL;
    serial->parent.tx_complete = FM_NULL;

    /* Call the control() API to close the serial device */
    serial->ops->control(serial, FM_DEVICE_CTRL_CLOSE, FM_NULL);
    dev->flag &= ~FM_DEVICE_FLAG_ACTIVATED;

    return FM_EOK;
}

/**
  * @brief Control the serial device.
  * @param dev The pointer of device driver structure
  * @param cmd The command value that controls the serial device
  * @param args The parameter value that controls the serial device
  * @return Return the status of the operation.
  */
static fm_err_t fm_serial_control(struct fm_device *dev,
                                  int               cmd,
                                  void             *args)
{
    fm_err_t ret = FM_EOK;
    struct fm_serial_device *serial;

    FM_ASSERT(dev != FM_NULL);
    serial = (struct fm_serial_device *)dev;

    switch (cmd)
    {
        case FM_DEVICE_CTRL_SUSPEND:
            /* suspend device */
            dev->flag |= FM_DEVICE_FLAG_SUSPENDED;
            break;

        case FM_DEVICE_CTRL_RESUME:
            /* resume device */
            dev->flag &= ~FM_DEVICE_FLAG_SUSPENDED;
            break;

        case FM_DEVICE_CTRL_CONFIG:
            if (args != FM_NULL)
            {
                struct serial_configure *pconfig = (struct serial_configure *) args;
                if (((pconfig->rx_bufsz != serial->config.rx_bufsz) || (pconfig->tx_bufsz != serial->config.tx_bufsz))
                        && serial->parent.ref_count)
                {
                    /*can not change buffer size*/
                    return -FM_EBUSY;
                }
                /* set serial configure */
                serial->config = *pconfig;
                serial->ops->configure(serial, (struct serial_configure *) args);
            }
            break;

        case FM_DEVICE_CTRL_CONSOLE_OFLAG:
            if (args)
            {
                *(fm_uint16_t*)args = FM_DEVICE_FLAG_RDWR | FM_DEVICE_FLAG_INT_RX | FM_DEVICE_FLAG_STREAM;
            }
            break;

        default :
            /* control device */
            ret = serial->ops->control(serial, cmd, args);
            break;
    }

    return ret;
}

#ifdef FM_USING_DEVICE_OPS
static fm_ssize_t fm_serial_read(struct fm_device *dev,
                                fm_off_t          pos,
                                void             *buffer,
                                fm_size_t         size)
{
    struct fm_serial_device *serial;

    FM_ASSERT(dev != FM_NULL);
    if (size == 0) return 0;

    serial = (struct fm_serial_device *)dev;

    if (serial->config.rx_bufsz)
    {
        return _serial_fifo_rx(dev, pos, buffer, size);
    }

    return _serial_poll_rx(dev, pos, buffer, size);
}


static fm_ssize_t fm_serial_write(struct fm_device *dev,
                                 fm_off_t          pos,
                                 const void       *buffer,
                                 fm_size_t         size)
{
    struct fm_serial_device *serial;
    struct fm_serial_tx_fifo *tx_fifo;

    FM_ASSERT(dev != FM_NULL);
    if (size == 0) return 0;

    serial = (struct fm_serial_device *)dev;
    FM_ASSERT((serial != FM_NULL) && (buffer != FM_NULL));
    tx_fifo = (struct fm_serial_tx_fifo *) serial->serial_tx;

    if (serial->config.tx_bufsz == 0)
    {
        return _serial_poll_tx(dev, pos, buffer, size);
    }

    if (dev->open_flag & FM_SERIAL_TX_BLOCKING)
    {
        if ((tx_fifo->rb.buffer_ptr) == FM_NULL)
        {
            return _serial_fifo_tx_blocking_nbuf(dev, pos, buffer, size);
        }

        return _serial_fifo_tx_blocking_buf(dev, pos, buffer, size);
    }

    return _serial_fifo_tx_nonblocking(dev, pos, buffer, size);
}

const static struct fm_device_ops serial_ops =
{
    fm_serial_init,
    fm_serial_open,
    fm_serial_close,
    fm_serial_read,
    fm_serial_write,
    fm_serial_control
};
#endif

/**
  * @brief Register the serial device.
  * @param serial RT-thread serial device.
  * @param name The device driver's name
  * @param flag The capabilities flag of device.
  * @param data The device driver's data.
  * @return Return the status of the operation.
  */
fm_err_t fm_hw_serial_register(struct fm_serial_device *serial,
                               const char              *name,
                               fm_uint32_t              flag,
                               void                    *data)
{
    fm_err_t ret;
    struct fm_device *device;
    FM_ASSERT(serial != FM_NULL);

    device = &(serial->parent);

    device->type        = FM_Device_Class_Char;
    device->rx_indicate = FM_NULL;
    device->tx_complete = FM_NULL;

#ifdef FM_USING_DEVICE_OPS
    device->ops         = &serial_ops;
#else
    device->init        = fm_serial_init;
    device->open        = fm_serial_open;
    device->close       = fm_serial_close;
    device->read        = FM_NULL;
    device->write       = FM_NULL;
    device->control     = fm_serial_control;
#endif
    device->user_data   = data;

    /* register a character device */
    ret = fm_device_register(device, name, flag);

    return ret;
}

/**
  * @brief ISR for serial interrupt
  * @param serial RT-thread serial device.
  * @param event ISR event type.
  */
void fm_hw_serial_isr(struct fm_serial_device *serial, int event)
{
    FM_ASSERT(serial != FM_NULL);

    switch (event & 0xff)
    {
        /* Interrupt receive event */
        case FM_SERIAL_EVENT_RX_IND:
        case FM_SERIAL_EVENT_RX_DMADONE:
        {
            struct fm_serial_rx_fifo *rx_fifo;
            fm_size_t rx_length = 0;
            rx_fifo = (struct fm_serial_rx_fifo *)serial->serial_rx;
            fm_base_t level;
            FM_ASSERT(rx_fifo != FM_NULL);

            /* If the event is FM_SERIAL_EVENT_RX_IND, rx_length is equal to 0 */
            rx_length = (event & (~0xff)) >> 8;

            if (rx_length)
            { /* FM_SERIAL_EVENT_RX_DMADONE MODE */
                level = fm_hw_interrupt_disable();
                fm_serial_update_write_index(&(rx_fifo->rb), rx_length);
                fm_hw_interrupt_enable(level);
            }

            /* Get the length of the data from the ringbuffer */
            rx_length = fm_ringbuffer_data_len(&rx_fifo->rb);
            if (rx_length == 0) break;

            if (serial->parent.open_flag & FM_SERIAL_RX_BLOCKING)
            {
                if (rx_fifo->rx_cpt_index && rx_length >= rx_fifo->rx_cpt_index )
                {
                    rx_fifo->rx_cpt_index = 0;
                    fm_completion_done(&(rx_fifo->rx_cpt));
                }
            }
            /* Trigger the receiving completion callback */
            if (serial->parent.rx_indicate != FM_NULL)
                serial->parent.rx_indicate(&(serial->parent), rx_length);

            break;
        }

        /* Interrupt transmit event */
        case FM_SERIAL_EVENT_TX_DONE:
        {
            struct fm_serial_tx_fifo *tx_fifo;
            fm_size_t tx_length = 0;
            tx_fifo = (struct fm_serial_tx_fifo *)serial->serial_tx;
            FM_ASSERT(tx_fifo != FM_NULL);

            /* Get the length of the data from the ringbuffer */
            tx_length = fm_ringbuffer_data_len(&tx_fifo->rb);
            /* If there is no data in tx_ringbuffer,
             * then the transmit completion callback is triggered*/
            if (tx_length == 0)
            {
                /* Trigger the transmit completion callback */
                if (serial->parent.tx_complete != FM_NULL)
                    serial->parent.tx_complete(&serial->parent, FM_NULL);

                /* Maybe some datas left in the buffer still need to be sent in block mode,
                 * so tx_fifo->activated should be FM_TRUE */
                if (serial->parent.open_flag & FM_SERIAL_TX_BLOCKING)
                {
                    fm_completion_done(&(tx_fifo->tx_cpt));
                }
                else
                {
                    tx_fifo->activated = FM_FALSE;
                }

                break;
            }

            /* Call the transmit interface for transmission again */
            /* Note that in interrupt mode, tx_fifo->buffer and tx_length
             * are inactive parameters */
            serial->ops->transmit(serial,
                                tx_fifo->buffer,
                                tx_length,
                                serial->parent.open_flag & ( \
                                FM_SERIAL_TX_BLOCKING | \
                                FM_SERIAL_TX_NON_BLOCKING));
            break;
        }

        case FM_SERIAL_EVENT_TX_DMADONE:
        {
            struct fm_serial_tx_fifo *tx_fifo;
            tx_fifo = (struct fm_serial_tx_fifo *)serial->serial_tx;
            FM_ASSERT(tx_fifo != FM_NULL);

            tx_fifo->activated = FM_FALSE;

            /* Trigger the transmit completion callback */
            if (serial->parent.tx_complete != FM_NULL)
                serial->parent.tx_complete(&serial->parent, FM_NULL);

            if (serial->parent.open_flag & FM_SERIAL_TX_BLOCKING)
            {
                fm_completion_done(&(tx_fifo->tx_cpt));
                break;
            }

            fm_serial_update_read_index(&tx_fifo->rb, tx_fifo->put_size);
            /* Get the length of the data from the ringbuffer.
             * If there is some data in tx_ringbuffer,
             * then call the transmit interface for transmission again */
            if (fm_ringbuffer_data_len(&tx_fifo->rb))
            {
                tx_fifo->activated = FM_TRUE;

                fm_uint8_t *put_ptr  = FM_NULL;
                /* Get the linear length buffer from rinbuffer */
                tx_fifo->put_size = fm_serial_get_linear_buffer(&(tx_fifo->rb), &put_ptr);
                /* Call the transmit interface for transmission again */
                serial->ops->transmit(serial,
                                    put_ptr,
                                    tx_fifo->put_size,
                                    FM_SERIAL_TX_NON_BLOCKING);
            }

            break;
        }

        default:
            break;
    }
}

