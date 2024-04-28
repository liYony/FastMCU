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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <driver.h>
#include <resource.h>

struct fm_platform_device
{
    struct fm_device parent;

    int dev_id;

    fm_uint32_t num_resources;
    struct fm_resource *resource;

    const char *name;

    void *priv;
};

struct fm_platform_driver
{
    struct fm_driver parent;

    const char *name;

    fm_err_t (*probe)(struct fm_platform_device *pdev);
    fm_err_t (*remove)(struct fm_platform_device *pdev);
    fm_err_t (*shutdown)(struct fm_platform_device *pdev);
};

struct fm_resource *fm_platform_get_resource(struct fm_platform_device *dev,
                                             unsigned int type, unsigned int num);
int fm_platform_get_irq(struct fm_platform_device *dev, unsigned int num);
struct fm_resource *fm_platform_get_resource_byname(struct fm_platform_device *dev,
                                                    unsigned int type,
                                                    const char *name);
int fm_platform_get_irq_byname(struct fm_platform_device *dev, const char *name);
fm_err_t fm_platform_add_devices(struct fm_platform_device **devs, int num);
fm_err_t fm_platform_device_add_resources(struct fm_platform_device *pdev,
                                          const struct fm_resource *res, unsigned int num);

struct fm_platform_device *fm_platform_device_alloc(const char *name);

fm_err_t fm_platform_driver_register(struct fm_platform_driver *pdrv);
fm_err_t fm_platform_device_register(struct fm_platform_device *pdev);

#define FM_PLATFORM_DRIVER_EXPORT(driver) FM_DRIVER_EXPORT(driver, platform, BUILIN)

#endif /* __PLATFORM_H__ */
