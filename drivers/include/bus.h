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

#ifndef __CORE_BUS_H__
#define __CORE_BUS_H__

#include <fmdef.h>
#include <driver.h>

typedef struct fm_bus *fm_bus_t;

struct fm_bus
{
    struct fm_object parent;    /**< inherit from fm_object */

    const char *name;

    fm_list_t list;
    fm_list_t dev_list;
    fm_list_t drv_list;

    fm_bool_t (*match)(fm_driver_t drv, fm_device_t dev);
    fm_err_t  (*probe)(fm_device_t dev);
    fm_err_t  (*remove)(fm_device_t dev);
    fm_err_t  (*shutdown)(fm_device_t dev);
};

fm_err_t fm_bus_for_each_dev(fm_bus_t bus, void *data, int (*fn)(fm_device_t dev, void *));
fm_err_t fm_bus_for_each_drv(fm_bus_t bus, void *data, int (*fn)(fm_driver_t drv, void *));

fm_err_t fm_bus_add_driver(fm_bus_t bus, fm_driver_t drv);
fm_err_t fm_bus_add_device(fm_bus_t bus, fm_device_t dev);
fm_err_t fm_bus_remove_driver(fm_driver_t drv);
fm_err_t fm_bus_remove_device(fm_device_t dev);

fm_err_t fm_bus_shutdown(void);

fm_bus_t fm_bus_find_by_name(const char *name);
fm_err_t fm_bus_reload_driver_device(fm_bus_t new_bus, fm_device_t dev);

fm_err_t fm_bus_register(fm_bus_t bus);

#endif /* __BUS_H__ */
