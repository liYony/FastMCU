/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2012-04-25     weety         first version
 * 2021-04-20     RiceChen      added bus clock command
 * 2023-12-20     liYony        Adaptive FastMCU
 */

#ifndef __I2C_DEV_H__
#define __I2C_DEV_H__

#include <fmcu.h>
#include <i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FM_I2C_DEV_CTRL_10BIT        (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x01)
#define FM_I2C_DEV_CTRL_ADDR         (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x02)
#define FM_I2C_DEV_CTRL_TIMEOUT      (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x03)
#define FM_I2C_DEV_CTRL_RW           (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x04)
#define FM_I2C_DEV_CTRL_CLK          (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x05)
#define FM_I2C_DEV_CTRL_UNLOCK       (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x06)
#define FM_I2C_DEV_CTRL_GET_STATE    (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x07)
#define FM_I2C_DEV_CTRL_GET_MODE     (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x08)
#define FM_I2C_DEV_CTRL_GET_ERROR    (FM_DEVICE_CTRL_BASE(I2CBUS) + 0x09)

struct fm_i2c_priv_data
{
    struct fm_i2c_msg  *msgs;
    fm_size_t  number;
};

fm_err_t fm_i2c_bus_device_device_init(struct fm_i2c_bus_device *bus,
                                       const char               *name);

#ifdef __cplusplus
}
#endif

#endif
