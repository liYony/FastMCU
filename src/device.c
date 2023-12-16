#include <fmcu.h>

#define DBG_TAG "device"
#define DBG_LVL DBG_LOG
#include <fmdbg.h>

static struct fm_list_node device_list_head = FM_LIST_OBJECT_INIT(device_list_head);

#ifdef FM_USING_DEVICE_OPS
#define device_init     (dev->ops->init)
#define device_open     (dev->ops->open)
#define device_close    (dev->ops->close)
#define device_read     (dev->ops->read)
#define device_write    (dev->ops->write)
#define device_control  (dev->ops->control)
#else
#define device_init     (dev->init)
#define device_open     (dev->open)
#define device_close    (dev->close)
#define device_read     (dev->read)
#define device_write    (dev->write)
#define device_control  (dev->control)
#endif /* FM_USING_DEVICE_OPS */

/**
 * @brief This function registers a device driver with a specified name.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param name is the device driver's name.
 *
 * @param flags is the capabilities flag of device.
 *
 * @return the error code, FM_EOK on initialization successfully.
 */
fm_err_t fm_device_register(fm_device_t dev,
                            const char *name,
                            fm_uint16_t flags)
{
    if (dev == FM_NULL)
        return -FM_ERROR;

    if (fm_device_find(name) != FM_NULL)
        return -FM_ERROR;
    
    fm_strncpy(dev->name, name, FM_NAME_MAX);  /* copy name */
    fm_list_insert_after(&device_list_head, &(dev->list));
    
    dev->flag = flags;
    dev->ref_count = 0;
    dev->open_flag = 0;

    return FM_EOK;
}

/**
 * @brief This function removes a previously registered device driver.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @return the error code, FM_EOK on successfully.
 */
fm_err_t fm_device_unregister(fm_device_t dev)
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);

    fm_list_remove(&(dev->list));

    return FM_EOK;
}


/**
 * @brief This function finds a device driver by specified name.
 *
 * @param name is the device driver's name.
 *
 * @return the registered device driver on successful, or FM_NULL on failure.
 */
fm_device_t fm_device_find(const char *name)
{
    struct fm_device *device = FM_NULL;
    struct fm_list_node *node = FM_NULL;

    if (name == FM_NULL) return FM_NULL;
    
    fm_list_for_each(node, &device_list_head)
    {
        device = fm_list_entry(node, struct fm_device, list);
        if (fm_strncmp(device->name, name, FM_NAME_MAX) == 0)
        {
            return device;
        }
    }
    
    return FM_NULL;
}

#ifdef FM_USING_HEAP
/**
 * @brief This function creates a device object with user data size.
 *
 * @param type is the type of the device object.
 *
 * @param attach_size is the size of user data.
 *
 * @return the allocated device object, or FM_NULL when failed.
 */
fm_device_t fm_device_create(int type, int attach_size)
{
    int size;
    fm_device_t device;

    size = FM_ALIGN(sizeof(struct fm_device), FM_ALIGN_SIZE);
    attach_size = FM_ALIGN(attach_size, FM_ALIGN_SIZE);
    /* use the total size */
    size += attach_size;

    device = (fm_device_t)fm_malloc(size);
    if (device)
    {
        fm_memset(device, 0x0, sizeof(struct fm_device));
        device->type = (enum fm_device_class_type)type;
    }

    return device;
}

/**
 * @brief This function destroy the specific device object.
 *
 * @param dev is a specific device object.
 */
void fm_device_destroy(fm_device_t dev)
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);
    
    fm_list_remove(&(dev->list));

    /* release this device object */
    fm_free(dev);
}
#endif /* FM_USING_HEAP */

/**
 * @brief This function will initialize the specified device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @return the result, FM_EOK on successfully.
 */
fm_err_t fm_device_init(fm_device_t dev)
{
    fm_err_t result = FM_EOK;

    FM_ASSERT(dev != FM_NULL);

    /* get device_init handler */
    if (device_init != FM_NULL)
    {
        if (!(dev->flag & FM_DEVICE_FLAG_ACTIVATED))
        {
            result = device_init(dev);
            if (result != FM_EOK)
            {
                LOG_E("To initialize device:%s failed. The error code is %d",
                      dev->name, result);
            }
            else
            {
                dev->flag |= FM_DEVICE_FLAG_ACTIVATED;
            }
        }
    }

    return result;
}

/**
 * @brief This function will open a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param oflag is the flags for device open.
 *
 * @return the result, FM_EOK on successfully.
 */
fm_err_t fm_device_open(fm_device_t dev, fm_uint16_t oflag)
{
    fm_err_t result = FM_EOK;

    /* parameter check */
    FM_ASSERT(dev != FM_NULL);
    
    /* if device is not initialized, initialize it. */
    if (!(dev->flag & FM_DEVICE_FLAG_ACTIVATED))
    {
        if (device_init != FM_NULL)
        {
            result = device_init(dev);
            if (result != FM_EOK)
            {
                LOG_E("To initialize device:%s failed. The error code is %d",
                      dev->name, result);

                return result;
            }
        }

        dev->flag |= FM_DEVICE_FLAG_ACTIVATED;
    }

    /* device is a stand alone device and opened */
    if ((dev->flag & FM_DEVICE_FLAG_STANDALONE) &&
        (dev->open_flag & FM_DEVICE_OFLAG_OPEN))
    {
        return -FM_EBUSY;
    }

    /* device is not opened or opened by other oflag, call device_open interface */
    if (!(dev->open_flag & FM_DEVICE_OFLAG_OPEN) ||
         ((dev->open_flag & FM_DEVICE_OFLAG_MASK) != (oflag & FM_DEVICE_OFLAG_MASK)))
    {
        if (device_open != FM_NULL)
        {
            result = device_open(dev, oflag);
        }
        else
        {
            /* set open flag */
            dev->open_flag = (oflag & FM_DEVICE_OFLAG_MASK);
        }
    }

    /* set open flag */
    if (result == FM_EOK || result == -FM_ENOSYS)
    {
        dev->open_flag |= FM_DEVICE_OFLAG_OPEN;

        dev->ref_count++;
        /* don't let bad things happen silently. If you are bitten by this assert,
        * please set the ref_count to a bigger type. */
        FM_ASSERT(dev->ref_count != 0);
    }

    return result;
}

/**
 * @brief This function will close a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @return the result, FM_EOK on successfully.
 */
fm_err_t fm_device_close(fm_device_t dev)
{
    fm_err_t result = FM_EOK;

    /* parameter check */
    FM_ASSERT(dev != FM_NULL);
    
    if (dev->ref_count == 0)
        return -FM_ERROR;

    dev->ref_count--;

    if (dev->ref_count != 0)
        return FM_EOK;

    /* call device_close interface */
    if (device_close != FM_NULL)
    {
        result = device_close(dev);
    }

    /* set open flag */
    if (result == FM_EOK || result == -FM_ENOSYS)
        dev->open_flag = FM_DEVICE_OFLAG_CLOSE;

    return result;
}

/**
 * @brief This function will read some data from a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param pos is the position when reading.
 *
 * @param buffer is a data buffer to save the read data.
 *
 * @param size is the size of buffer.
 *
 * @return the actually read size on successful, otherwise 0 will be returned.
 *
 * @note the unit of size/pos is a block for block device.
 */
fm_ssize_t fm_device_read(fm_device_t dev,
                         fm_off_t    pos,
                         void       *buffer,
                         fm_size_t   size)
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);

    if (dev->ref_count == 0)
    {
        return 0;
    }

    /* call device_read interface */
    if (device_read != FM_NULL)
    {
        return device_read(dev, pos, buffer, size);
    }

    return 0;
}

/**
 * @brief This function will write some data to a device.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param pos is the position when writing.
 *
 * @param buffer is the data buffer to be written to device.
 *
 * @param size is the size of buffer.
 *
 * @return the actually written size on successful, otherwise 0 will be returned.
 *
 * @note the unit of size/pos is a block for block device.
 */
fm_ssize_t fm_device_write(fm_device_t dev,
                          fm_off_t    pos,
                          const void *buffer,
                          fm_size_t   size)
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);

    if (dev->ref_count == 0)
    {
        return 0;
    }

    /* call device_write interface */
    if (device_write != FM_NULL)
    {
        return device_write(dev, pos, buffer, size);
    }

    return 0;
}

/**
 * @brief This function will perform a variety of control functions on devices.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param cmd is the command sent to device.
 *
 * @param arg is the argument of command.
 *
 * @return the result, -FM_ENOSYS for failed.
 */
fm_err_t fm_device_control(fm_device_t dev, int cmd, void *arg)
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);

    /* call device_write interface */
    if (device_control != FM_NULL)
    {
        return device_control(dev, cmd, arg);
    }

    return -FM_ENOSYS;
}

/**
 * @brief This function will set the reception indication callback function. This callback function
 *        is invoked when this device receives data.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param rx_ind is the indication callback function.
 *
 * @return FM_EOK
 */
fm_err_t fm_device_set_rx_indicate(fm_device_t dev,
                                   fm_err_t (*rx_ind)(fm_device_t dev,
                                   fm_size_t size))
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);

    dev->rx_indicate = rx_ind;

    return FM_EOK;
}

/**
 * @brief This function will set a callback function. The callback function
 *        will be called when device has written data to physical hardware.
 *
 * @param dev is the pointer of device driver structure.
 *
 * @param tx_done is the indication callback function.
 *
 * @return FM_EOK
 */
fm_err_t fm_device_set_tx_complete(fm_device_t dev,
                                   fm_err_t (*tx_done)(fm_device_t dev,
                                   void *buffer))
{
    /* parameter check */
    FM_ASSERT(dev != FM_NULL);

    dev->tx_complete = tx_done;

    return FM_EOK;
}
