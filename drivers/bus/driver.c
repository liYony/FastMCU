/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Change Logs:
 * 2024-04-27     liYony        Adaptive FastMCU
 */

#include <fmcu.h>
#include <bus.h>

/**
 * This function attach a driver to bus
 *
 * @param drv the driver to be attached
 */
fm_err_t fm_driver_register(fm_driver_t drv)
{
    fm_err_t ret;
    struct fm_bus *bus = FM_NULL;

    FM_ASSERT(drv != FM_NULL);

    if (drv->bus)
    {
        bus = drv->bus;
        ret = fm_bus_add_driver(bus, drv);
    }
    else
    {
        ret = -FM_EINVAL;
    }

    return ret;
}

/**
 * This function remove driver from system.
 *
 * @param drv the driver to be removed
 */
fm_err_t fm_driver_unregister(fm_driver_t drv)
{
    fm_err_t ret;

    ret = fm_bus_remove_driver(drv);

    return ret;
}
