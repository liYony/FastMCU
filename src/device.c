#include <fmcu.h>

static struct fm_list_node device_list_head = FM_LIST_OBJECT_INIT(device_list_head);

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
    if (dev == FM_NULL)
        return -FM_ERROR;

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
