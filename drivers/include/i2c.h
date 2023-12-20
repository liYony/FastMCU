/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2012-04-25     weety         first version
 * 2021-04-20     RiceChen      added support for bus control api
 * 2023-12-20     liYony        Adaptive FastMCU
 */

#ifndef __I2C_H__
#define __I2C_H__

#include <fmcu.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FM_I2C_WR                0x0000
#define FM_I2C_RD               (1u << 0)
#define FM_I2C_ADDR_10BIT       (1u << 2)  /* this is a ten bit chip address */
#define FM_I2C_NO_START         (1u << 4)
#define FM_I2C_IGNORE_NACK      (1u << 5)
#define FM_I2C_NO_READ_ACK      (1u << 6)  /* when I2C reading, we do not ACK */
#define FM_I2C_NO_STOP          (1u << 7)

struct fm_i2c_msg
{
    fm_uint16_t addr;
    fm_uint16_t flags;
    fm_uint16_t len;
    fm_uint8_t  *buf;
};

struct fm_i2c_bus_device;

struct fm_i2c_bus_device_ops
{
    fm_ssize_t (*master_xfer)(struct fm_i2c_bus_device *bus,
                             struct fm_i2c_msg msgs[],
                             fm_uint32_t num);
    fm_ssize_t (*slave_xfer)(struct fm_i2c_bus_device *bus,
                            struct fm_i2c_msg msgs[],
                            fm_uint32_t num);
    fm_err_t (*i2c_bus_control)(struct fm_i2c_bus_device *bus,
                                int cmd,
                                void *args);
};

/*for i2c bus driver*/
struct fm_i2c_bus_device
{
    struct fm_device parent;
    const struct fm_i2c_bus_device_ops *ops;
    fm_uint16_t  flags;
    fm_uint32_t  timeout;
    fm_uint32_t  retries;
    void *priv;
};

struct fm_i2c_client
{
    struct fm_i2c_bus_device       *bus;
    fm_uint16_t                    client_addr;
};

fm_err_t fm_i2c_bus_device_register(struct fm_i2c_bus_device *bus,
                                    const char               *bus_name);
struct fm_i2c_bus_device *fm_i2c_bus_device_find(const char *bus_name);
fm_ssize_t fm_i2c_transfer(struct fm_i2c_bus_device *bus,
                          struct fm_i2c_msg         msgs[],
                          fm_uint32_t               num);
fm_err_t fm_i2c_control(struct fm_i2c_bus_device *bus,
                        int cmd,
                        void *args);
fm_ssize_t fm_i2c_master_send(struct fm_i2c_bus_device *bus,
                             fm_uint16_t               addr,
                             fm_uint16_t               flags,
                             const fm_uint8_t         *buf,
                             fm_uint32_t               count);
fm_ssize_t fm_i2c_master_recv(struct fm_i2c_bus_device *bus,
                             fm_uint16_t               addr,
                             fm_uint16_t               flags,
                             fm_uint8_t               *buf,
                             fm_uint32_t               count);

#ifdef __cplusplus
}
#endif

#endif
