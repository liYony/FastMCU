/*
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-04-28     liYony       the first version
 */

#include <fmdevice.h>

#define DBG_TAG "e.pltaform"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

static fm_err_t e_driver_probe(struct fm_platform_device *pdev)
{
    fm_err_t err = FM_EOK;

    struct fm_resource *res;
    
    res = fm_platform_get_resource(pdev, FM_IORESOURCE_MEM, 2);
    
    LOG_I("resource.name  = %s", res->name);
    LOG_I("resource.start = 0x%p", res->start);
    LOG_I("resource.end   = 0x%p", res->end);
    
    res = fm_platform_get_resource_byname(pdev, FM_IORESOURCE_MEM, "resource1");
    
    LOG_I("resource.name  = %s", res->name);
    LOG_I("resource.start = 0x%p", res->start);
    LOG_I("resource.end   = 0x%p", res->end);
    
    return err;
}

static struct fm_platform_driver e_driver =
{
    .name = "e_platform",

    .probe = e_driver_probe,
};

FM_PLATFORM_DRIVER_EXPORT(e_driver)

struct fm_resource e_resources[] = 
{
    [0] = {
        .name = "resource0",
        .start = 0x10000000,
        .end = 0x20000000,
        .flags = FM_IORESOURCE_MEM,
    },
    [1] = {
        .name = "resource1",
        .start = 0x30000000,
        .end = 0x40000000,
        .flags = FM_IORESOURCE_MEM,
    },
    [2] = {
        .name = "resource2",
        .start = 0x50000000,
        .end = 0x60000000,
        .flags = FM_IORESOURCE_MEM,
    }
};

struct fm_platform_device *e_device = FM_NULL;

static int e_platform_device_init(void)
{
    e_device = fm_platform_device_alloc("e_platform");
    
    e_device->resource = e_resources;
    e_device->num_resources = FM_ARRAY_SIZE(e_resources);
    
    fm_platform_device_register(e_device);
    
    return 0;
}

INIT_ENV_EXPORT(e_platform_device_init);
