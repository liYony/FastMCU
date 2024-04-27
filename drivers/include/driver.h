/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-12     ErikChan      the first version
 * 2024-04-27     liYony        Adaptive FastMCU
 */

#ifndef __CORE_DRIVER_H__
#define __CORE_DRIVER_H__

#include <fmdef.h>

struct fm_driver
{
    struct fm_object parent;
    struct fm_bus *bus;
    fm_list_t node;

    fm_uint32_t ref_count;

    /* common device interface */
    fm_err_t  (*init)   (fm_device_t dev);
    fm_err_t  (*open)   (fm_device_t dev, fm_uint16_t oflag);
    fm_err_t  (*close)  (fm_device_t dev);
    fm_ssize_t (*read)  (fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size);
    fm_ssize_t (*write) (fm_device_t dev, fm_off_t pos, const void *buffer, fm_size_t size);
    fm_err_t  (*control)(fm_device_t dev, int cmd, void *args);

    const struct filesystem_ops *fops;

    int (*probe)(struct fm_device *dev);
    int (*remove)(struct fm_device *dev);
    int (*shutdown)(struct fm_device *dev);
};

fm_err_t fm_driver_register(fm_driver_t drv);
fm_err_t fm_driver_unregister(fm_driver_t drv);

#define FM_DRIVER_EXPORT(driver, bus_name, mode)    \
static int ___##driver##_register(void)             \
{                                                   \
    fm_##bus_name##_driver_register(&driver);       \
    return 0;                                       \
}                                                   \
INIT_DEVICE_EXPORT(___##driver##_register);         \

#endif /* __DRIVER_H__ */
