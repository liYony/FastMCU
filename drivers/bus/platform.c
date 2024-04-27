/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-12     ErikChan      the first version
 * 2023-10-13     zmshahaha     distinguish ofw and none-ofw situation
 * 2024-04-27     liYony        Adaptive FastMCU
 */

#include <fmcu.h>

#define DBG_TAG "Pltaform"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

#include <platform.h>
#include <bus.h>

static struct fm_bus platform_bus;

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
