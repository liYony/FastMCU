/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-12     ErikChan     the first version
 * 2023-10-13     zmshahaha    distinguish ofw and none-ofw situation
 * 2024-04-27     liYony       Adaptive FastMCU
 * 2024-04-28     liYony       add platform resource
 */

#include <fmcu.h>

#define DBG_TAG "Pltaform"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

#include <platform.h>
#include <bus.h>

static struct fm_bus platform_bus;

/**
 *  @brief This function get a resource for a device.
 *
 *  @param dev platform device.
 *  @param type resource type.
 *  @param num resource index.
 *
 *  @return platform device resource.
 */
struct fm_resource *fm_platform_get_resource(struct fm_platform_device *dev,
                                             unsigned int type, unsigned int num)
{
    int i;

    for (i = 0; i < dev->num_resources; i++)
    {
        struct fm_resource *r = &dev->resource[i];

        if (type == fm_resource_type(r) && num-- == 0)
            return r;
    }

    return FM_NULL;
}

/**
 *  @brief This function get an IRQ for a device
 *
 *  @param dev platform device.
 *  @param num IRQ number index.
 *
 *  @return irq info
 */
int fm_platform_get_irq(struct fm_platform_device *dev, unsigned int num)
{
    struct fm_resource *r;

    r = fm_platform_get_resource(dev, FM_IORESOURCE_IRQ, num);

    return r ? r->start : -FM_EIO;
}

/**
 *  @brief This function get a resource for a device by name
 *
 *  @param dev platform device.
 *  @param type resource type.
 *  @param name resource name.
 *
 *  @return platform device resource.
 */
struct fm_resource *fm_platform_get_resource_byname(struct fm_platform_device *dev,
                                                    unsigned int type,
                                                    const char *name)
{
    int i;

    for (i = 0; i < dev->num_resources; i++)
    {
        struct fm_resource *r = &dev->resource[i];

        if (!r->name)
            continue;

        if (type == fm_resource_type(r) && !fm_strcmp(r->name, name))
            return r;
    }

    return FM_NULL;
}

/**
 *  @brief This function get an IRQ for a device by name.
 *
 *  @param dev platform device.
 *  @param name IRQ name.
 *
 *  @return irq info
 */
int fm_platform_get_irq_byname(struct fm_platform_device *dev, const char *name)
{
    struct fm_resource *r;

    r = fm_platform_get_resource_byname(dev, FM_IORESOURCE_IRQ, name);

    return r ? r->start : -FM_EIO;
}

/**
 *  @brief This function add a numbers of platform devices.
 *
 *  @param devs array of platform devices to add.
 *  @param num number of platform devices in array.
 *
 *  @return the error code, FM_EOK on successfully.
 */
fm_err_t fm_platform_add_devices(struct fm_platform_device **devs, int num)
{
    fm_err_t ret = FM_EOK;

    int i;
    for (i = 0; i < num; i++)
    {
        ret = fm_platform_device_register(devs[i]);
        if (ret)
        {
            LOG_E("device %s register failed.", devs[i]->name);
            break;
        }
    }

    return ret;
}

/**
 *  @brief This function add resources to a platform device.
 *
 *  @param pdev platform device allocated by platform_device_alloc to add resources to.
 *  @param res set of resources that needs to be allocated for the device.
 *  @param num number of resources.
 *
 *  @return the error code, FM_EOK on successfully.
 */
fm_err_t fm_platform_device_add_resources(struct fm_platform_device *pdev,
                                          const struct fm_resource *res, unsigned int num)
{
    struct fm_resource *r = FM_NULL;

    if (res)
    {
        r = fm_memdup(res, sizeof(struct fm_resource) * num);
        if (!r)
            return -FM_ENOMEM;
    }

    fm_free(pdev->resource);
    pdev->resource = r;
    pdev->num_resources = num;

    return FM_EOK;
}

/**
 *  @brief This function create a platform device.
 *
 *  @param name is name of the platform device.
 *
 *  @return a new platform device.
 */
struct fm_platform_device *fm_platform_device_alloc(const char *name)
{
    struct fm_platform_device *pdev = fm_calloc(1, sizeof(*pdev));

    if (!pdev)
    {
        return FM_NULL;
    }

    pdev->parent.bus = &platform_bus;
    pdev->name = name;

    return pdev;
}

/**
 *  @brief This function register a fm_driver to platform bus.
 *
 *  @return the error code, FM_EOK on successfully.
 */
fm_err_t fm_platform_driver_register(struct fm_platform_driver *pdrv)
{
    FM_ASSERT(pdrv != FM_NULL);

    pdrv->parent.bus = &platform_bus;
#if FM_NAME_MAX > 0
    fm_strcpy(pdrv->parent.parent.name, pdrv->name);
#else
    pdrv->parent.parent.name = pdrv->name;
#endif
    return fm_driver_register(&pdrv->parent);
}

/**
 *  @brief This function register a fm_device to platform bus.
 *
 *  @return the error code, FM_EOK on successfully.
 */
fm_err_t fm_platform_device_register(struct fm_platform_device *pdev)
{
    FM_ASSERT(pdev != FM_NULL);

    return fm_bus_add_device(&platform_bus, &pdev->parent);
}

static fm_bool_t platform_match(fm_driver_t drv, fm_device_t dev)
{
    struct fm_platform_driver *pdrv = fm_container_of(drv, struct fm_platform_driver, parent);
    struct fm_platform_device *pdev = fm_container_of(dev, struct fm_platform_device, parent);

    /* match with name */
    if (pdev->name && pdrv->name)
    {
        if (pdev->name == pdrv->name)
        {
            return FM_TRUE;
        }
        else
        {
            return !fm_strcmp(pdrv->name, pdev->name);
        }
    }

    return FM_FALSE;
}

static fm_err_t platform_probe(fm_device_t dev)
{
    fm_err_t err;
    struct fm_platform_driver *pdrv = fm_container_of(dev->drv, struct fm_platform_driver, parent);
    struct fm_platform_device *pdev = fm_container_of(dev, struct fm_platform_device, parent);

    err = pdrv->probe(pdev);

    if (!err)
    {
        ;
    }
    else
    {
        if (err == -FM_ENOMEM)
        {
            LOG_W("System not memory in driver %s", pdrv->name);
        }
    }

    return err;
}

static struct fm_bus platform_bus =
{
    .name = "platform",
    .match = platform_match,
    .probe = platform_probe,
};

static int platform_bus_init(void)
{
    fm_bus_register(&platform_bus);

    return 0;
}

INIT_CORE_EXPORT(platform_bus_init);
