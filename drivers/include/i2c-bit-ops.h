/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2012-04-25     weety         first version
 * 2023-12-20     liYony        Adaptive FastMCU
 */

#ifndef __I2C_BIT_OPS_H__
#define __I2C_BIT_OPS_H__

#include <fmcu.h>
#include <i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fm_i2c_bit_ops
{
    void *data;            /* private data for lowlevel routines */
    void (*set_sda)(void *data, fm_int32_t state);
    void (*set_scl)(void *data, fm_int32_t state);
    fm_int32_t (*get_sda)(void *data);
    fm_int32_t (*get_scl)(void *data);

    void (*udelay)(fm_uint32_t us);

    fm_uint32_t delay_us;  /* scl and sda line delay */
    fm_uint32_t timeout;   /* in tick */
};

fm_err_t fm_i2c_bit_add_bus(struct fm_i2c_bus_device *bus,
                            const char               *bus_name);

#ifdef __cplusplus
}
#endif

#endif
