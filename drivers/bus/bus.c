/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-10-13     flybreak     the first version
 * 2023-04-12     ErikChan     support fm_bus
 * 2024-04-27     liYony        Adaptive FastMCU
 */

#include <fmcu.h>
#include <bus.h>
#include <string.h>
#include <stdlib.h>

#define DBG_TAG "Bus"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

static fm_list_t bus_nodes = FM_LIST_OBJECT_INIT(bus_nodes);

fm_device_t fm_device_bus_create(char *name, int attach_size)
{
    fm_err_t result = FM_EOK;
    fm_device_t dev = fm_device_create(FM_Device_Class_Bus, 0);

    result = fm_device_register(dev, name, FM_DEVICE_FLAG_RDWR | FM_DEVICE_FLAG_REMOVABLE);
    if (result < 0)
    {
        fm_kprintf("dev bus [%s] register failed!, ret=%d\n", name, result);
        return FM_NULL;
    }

    LOG_D("bus create");
    return dev;
}

fm_err_t fm_device_bus_destroy(fm_device_t dev)
{
    fm_device_unregister(dev);
    dev->parent.type = FM_Object_Class_Device;
    fm_device_destroy(dev);
    LOG_D("bus destroy");
    return FM_EOK;
}

/**
 *  @brief This function loop the dev_list of the bus, and call fn in each loop
 *
 *  @param bus the target bus
 *
 *  @param data the data push when call fn
 *
 *  @param fn  the function callback in each loop
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_for_each_dev(fm_bus_t bus, void *data, int (*fn)(fm_device_t dev, void *))
{
    fm_device_t dev;
    fm_err_t err = -FM_EEMPTY;
    fm_list_t *dev_list;
    struct fm_spinlock *dev_lock;

    FM_ASSERT(bus != FM_NULL);

    dev_list = &bus->dev_list;

    dev = fm_list_entry(dev_list->next, struct fm_device, node);

    while (&dev->node != dev_list)
    {
        if (!fn(dev, data))
        {
            err = FM_EOK;

            break;
        }

        dev = fm_list_entry(dev->node.next, struct fm_device, node);
    }

    return err;
}

/**
 *  @brief This function loop the drv_list of the bus, and call fn in each loop
 *
 *  @param bus the target bus
 *
 *  @param data the data push when call fn
 *
 *  @param fn  the function callback in each loop
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_for_each_drv(fm_bus_t bus, void *data, int (*fn)(fm_driver_t drv, void *))
{
    fm_driver_t drv;
    fm_err_t err = -FM_EEMPTY;
    fm_list_t *drv_list;
    struct fm_spinlock *drv_lock;

    FM_ASSERT(bus != FM_NULL);

    drv_list = &bus->drv_list;

    drv = fm_list_entry(drv_list->next, struct fm_driver, node);

    while (&drv->node != drv_list)
    {
        if (!fn(drv, data))
        {
            err = FM_EOK;

            break;
        }

        drv = fm_list_entry(drv->node.next, struct fm_driver, node);
    }

    return err;
}

static fm_err_t bus_probe(fm_driver_t drv, fm_device_t dev)
{
    fm_bus_t bus = drv->bus;
    fm_err_t err = -FM_EEMPTY;

    if (!bus)
    {
        bus = dev->bus;
    }

    if (!dev->drv && bus->match(drv, dev))
    {
        dev->drv = drv;

        err = bus->probe(dev);

        if (err)
        {
            dev->drv = FM_NULL;
        }
    }

    return err;
}

static int bus_probe_driver(fm_device_t dev, void *drv_ptr)
{
    bus_probe(drv_ptr, dev);

    /*
     * The driver is shared by multiple devices,
     * so we always return the '1' to enumerate all devices.
     */
    return 1;
}

static int bus_probe_device(fm_driver_t drv, void *dev_ptr)
{
    fm_err_t err;

    err = bus_probe(drv, dev_ptr);

    if (!err)
    {
        fm_bus_t bus = drv->bus;

        ++drv->ref_count;
    }

    return err;
}

/**
 *  @brief This function add a driver to the drv_list of a specific bus
 *
 *  @param bus the bus to add
 *
 *  @param drv the driver to be added
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_add_driver(fm_bus_t bus, fm_driver_t drv)
{
    FM_ASSERT(bus != FM_NULL);
    FM_ASSERT(drv != FM_NULL);

    drv->bus = bus;
    fm_list_init(&drv->node);

    fm_list_insert_before(&bus->drv_list, &drv->node);

    fm_bus_for_each_dev(bus, drv, bus_probe_driver);

    return FM_EOK;
}

/**
 *  @brief This function add a device to the dev_list of a specific bus
 *
 *  @param bus the bus to add
 *
 *  @param dev the device to be added
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_add_device(fm_bus_t bus, fm_device_t dev)
{
    FM_ASSERT(bus != FM_NULL);
    FM_ASSERT(dev != FM_NULL);

    dev->bus = bus;
    fm_list_init(&dev->node);

    fm_list_insert_before(&bus->dev_list, &dev->node);

    fm_bus_for_each_drv(bus, dev, bus_probe_device);

    return FM_EOK;
}

/**
 *  @brief This function remove a driver from bus
 *
 *  @param drv the driver to be removed
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_remove_driver(fm_driver_t drv)
{
    fm_err_t err;
    fm_bus_t bus;

    FM_ASSERT(drv != FM_NULL);
    FM_ASSERT(drv->bus != FM_NULL);

    bus = drv->bus;

    LOG_D("Bus(%s) remove driver %s", bus->name, drv->parent.name);

    if (drv->ref_count)
    {
        err = -FM_EBUSY;
    }
    else
    {
        fm_list_remove(&drv->node);
        err = FM_EOK;
    }

    return err;
}

/**
 *  @brief This function remove a device from bus
 *
 *  @param dev the device to be removed
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_remove_device(fm_device_t dev)
{
    fm_bus_t bus;
    fm_driver_t drv;
    fm_err_t err = FM_EOK;

    FM_ASSERT(dev != FM_NULL);
    FM_ASSERT(dev->bus != FM_NULL);

    bus = dev->bus;
    drv = dev->drv;

    LOG_D("Bus(%s) remove device %s", bus->name, dev->parent.name);

    fm_list_remove(&dev->node);

    if (dev->bus->remove)
    {
        err = dev->bus->remove(dev);
    }
    else if (drv)
    {
        if (drv->shutdown)
        {
            err = drv->shutdown(dev);
        }

        /* device and driver are in the same bus */
        --drv->ref_count;
    }

    return err;
}

struct bus_shutdown_info
{
    fm_bus_t bus;

    fm_err_t err;
};

static int device_shutdown(fm_device_t dev, void *info_ptr)
{
    fm_bus_t bus;
    fm_err_t err = FM_EOK;
    struct bus_shutdown_info *info = info_ptr;

    bus = info->bus;

    if (bus->shutdown)
    {
        LOG_D("Device(%s) shutdown", dev->parent.name);
        err = bus->shutdown(dev);
        LOG_D("  Result: %s", fm_strerror(err));
    }
    else if (dev->drv && dev->drv->shutdown)
    {
        LOG_D("Device(%s) shutdown", dev->parent.name);
        err = dev->drv->shutdown(dev);
        LOG_D("  Result: %s", fm_strerror(err));
    }

    if (err)
    {
        /* Only get the last one while system not crash */
        info->err = err;
    }

    /* Go on, we want to ask all devices to shutdown */
    return 1;
}

/**
 *  @brief This function call all buses' shutdown
 *
 *  @return the error code, FM_EOK on shutdown successfully.
 */
fm_err_t fm_bus_shutdown(void)
{
    fm_bus_t bus = FM_NULL;
    struct bus_shutdown_info info =
    {
        .err = FM_EOK,
    };

    fm_list_for_each_entry(bus, &bus_nodes, list)
    {
        info.bus = bus;
        fm_bus_for_each_dev(bus, &info, device_shutdown);
    }

    return info.err;
}

/**
 *  @brief This function find a bus by name
 *  @param bus the name to be finded
 *
 *  @return the bus finded by name.
 */
fm_bus_t fm_bus_find_by_name(const char *name)
{
    fm_bus_t bus = FM_NULL;

    FM_ASSERT(name != FM_NULL);

    fm_list_for_each_entry(bus, &bus_nodes, list)
    {
        if (!fm_strncmp(bus->name, name, FM_NAME_MAX))
        {
            break;
        }
    }

    return bus;
}

/**
 *  @brief This function transfer dev_list and drv_list to the other bus
 *
 *  @param new_bus the bus to transfer
 *
 *  @param dev the target device
 *
 *  @return the error code, FM_EOK on added successfully.
 */
fm_err_t fm_bus_reload_driver_device(fm_bus_t new_bus, fm_device_t dev)
{
    fm_bus_t old_bus;

    FM_ASSERT(new_bus != FM_NULL);
    FM_ASSERT(dev != FM_NULL);
    FM_ASSERT(dev->bus != FM_NULL);
    FM_ASSERT(dev->bus != new_bus);

    old_bus = dev->bus;

    fm_list_remove(&dev->node);

    return fm_bus_add_device(new_bus, dev);
}

/**
 *  @brief This function register a bus
 *  @param bus the bus to be registered
 *
 *  @return the error code, FM_EOK on registeration successfully.
 */
fm_err_t fm_bus_register(fm_bus_t bus)
{
    FM_ASSERT(bus != FM_NULL);

    fm_list_init(&bus->list);
    fm_list_init(&bus->dev_list);
    fm_list_init(&bus->drv_list);

    fm_list_insert_before(&bus_nodes, &bus->list);

    return FM_EOK;
}
