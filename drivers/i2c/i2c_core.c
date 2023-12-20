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

#include <fmdevice.h>

#define DBG_TAG               "I2C"
#ifdef FM_I2C_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#include <fmdbg.h>

fm_err_t fm_i2c_bus_device_register(struct fm_i2c_bus_device *bus,
                                    const char               *bus_name)
{
    fm_err_t res = FM_EOK;

    if (bus->timeout == 0) bus->timeout = FM_TICK_PER_SECOND;

    res = fm_i2c_bus_device_device_init(bus, bus_name);

    LOG_I("I2C bus [%s] registered", bus_name);

    return res;
}

struct fm_i2c_bus_device *fm_i2c_bus_device_find(const char *bus_name)
{
    struct fm_i2c_bus_device *bus;
    fm_device_t dev = fm_device_find(bus_name);
    if (dev == FM_NULL || dev->type != FM_Device_Class_I2CBUS)
    {
        LOG_E("I2C bus %s not exist", bus_name);

        return FM_NULL;
    }

    bus = (struct fm_i2c_bus_device *)dev->user_data;

    return bus;
}

fm_ssize_t fm_i2c_transfer(struct fm_i2c_bus_device *bus,
                          struct fm_i2c_msg         msgs[],
                          fm_uint32_t               num)
{
    fm_ssize_t ret;

    if (bus->ops->master_xfer)
    {
#ifdef FM_I2C_DEBUG
        for (ret = 0; ret < num; ret++)
        {
            LOG_D("msgs[%d] %c, addr=0x%02x, len=%d", ret,
                  (msgs[ret].flags & FM_I2C_RD) ? 'R' : 'W',
                  msgs[ret].addr, msgs[ret].len);
        }
#endif
        ret = bus->ops->master_xfer(bus, msgs, num);

        return ret;
    }
    else
    {
        LOG_E("I2C bus operation not supported");
        return -FM_EINVAL;
    }
}

fm_err_t fm_i2c_control(struct fm_i2c_bus_device *bus,
                        int                       cmd,
                        void                      *args)
{
    fm_err_t ret;

    if(bus->ops->i2c_bus_control)
    {
        ret = bus->ops->i2c_bus_control(bus, cmd, args);
        return ret;
    }
    else
    {
        LOG_E("I2C bus operation not supported");
        return -FM_EINVAL;
    }
}

fm_ssize_t fm_i2c_master_send(struct fm_i2c_bus_device *bus,
                             fm_uint16_t               addr,
                             fm_uint16_t               flags,
                             const fm_uint8_t         *buf,
                             fm_uint32_t               count)
{
    fm_ssize_t ret;
    struct fm_i2c_msg msg;

    msg.addr  = addr;
    msg.flags = flags;
    msg.len   = count;
    msg.buf   = (fm_uint8_t *)buf;

    ret = fm_i2c_transfer(bus, &msg, 1);

    return (ret == 1) ? count : ret;
}

fm_ssize_t fm_i2c_master_recv(struct fm_i2c_bus_device *bus,
                             fm_uint16_t               addr,
                             fm_uint16_t               flags,
                             fm_uint8_t               *buf,
                             fm_uint32_t               count)
{
    fm_ssize_t ret;
    struct fm_i2c_msg msg;
    FM_ASSERT(bus != FM_NULL);

    msg.addr   = addr;
    msg.flags  = flags | FM_I2C_RD;
    msg.len    = count;
    msg.buf    = buf;

    ret = fm_i2c_transfer(bus, &msg, 1);

    return (ret == 1) ? count : ret;
}
