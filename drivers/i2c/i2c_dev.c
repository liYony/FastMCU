/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2012-04-25     weety         first version
 * 2014-08-03     bernard       fix some compiling warning
 * 2021-04-20     RiceChen      added support for bus clock control
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

static fm_ssize_t i2c_bus_device_read(fm_device_t dev,
                                     fm_off_t    pos,
                                     void       *buffer,
                                     fm_size_t   count)
{
    fm_uint16_t addr;
    fm_uint16_t flags;
    struct fm_i2c_bus_device *bus = (struct fm_i2c_bus_device *)dev->user_data;

    FM_ASSERT(bus != FM_NULL);
    FM_ASSERT(buffer != FM_NULL);

    LOG_D("I2C bus dev [%s] reading %u bytes.", dev->parent.name, count);

    addr = pos & 0xffff;
    flags = (pos >> 16) & 0xffff;

    return fm_i2c_master_recv(bus, addr, flags, (fm_uint8_t *)buffer, count);
}

static fm_ssize_t i2c_bus_device_write(fm_device_t dev,
                                      fm_off_t    pos,
                                      const void *buffer,
                                      fm_size_t   count)
{
    fm_uint16_t addr;
    fm_uint16_t flags;
    struct fm_i2c_bus_device *bus = (struct fm_i2c_bus_device *)dev->user_data;

    FM_ASSERT(bus != FM_NULL);
    FM_ASSERT(buffer != FM_NULL);

    LOG_D("I2C bus dev [%s] writing %u bytes.", dev->parent.name, count);

    addr = pos & 0xffff;
    flags = (pos >> 16) & 0xffff;

    return fm_i2c_master_send(bus, addr, flags, (const fm_uint8_t *)buffer, count);
}

static fm_err_t i2c_bus_device_control(fm_device_t dev,
                                       int         cmd,
                                       void       *args)
{
    fm_err_t ret;
    struct fm_i2c_priv_data *priv_data;
    struct fm_i2c_bus_device *bus = (struct fm_i2c_bus_device *)dev->user_data;

    FM_ASSERT(bus != FM_NULL);

    switch (cmd)
    {
    /* set 10-bit addr mode */
    case FM_I2C_DEV_CTRL_10BIT:
        bus->flags |= FM_I2C_ADDR_10BIT;
        break;
    case FM_I2C_DEV_CTRL_TIMEOUT:
        bus->timeout = *(fm_uint32_t *)args;
        break;
    case FM_I2C_DEV_CTRL_RW:
        priv_data = (struct fm_i2c_priv_data *)args;
        ret = fm_i2c_transfer(bus, priv_data->msgs, priv_data->number);
        if (ret < 0)
        {
            return -FM_EIO;
        }
        break;
    default:
        return fm_i2c_control(bus, cmd, args);
    }

    return FM_EOK;
}

#ifdef FM_USING_DEVICE_OPS
const static struct fm_device_ops i2c_ops =
{
    FM_NULL,
    FM_NULL,
    FM_NULL,
    i2c_bus_device_read,
    i2c_bus_device_write,
    i2c_bus_device_control
};
#endif

fm_err_t fm_i2c_bus_device_device_init(struct fm_i2c_bus_device *bus,
                                       const char               *name)
{
    struct fm_device *device;
    FM_ASSERT(bus != FM_NULL);

    device = &bus->parent;

    device->user_data = bus;

    /* set device type */
    device->type    = FM_Device_Class_I2CBUS;
    /* initialize device interface */
#ifdef FM_USING_DEVICE_OPS
    device->ops     = &i2c_ops;
#else
    device->init    = FM_NULL;
    device->open    = FM_NULL;
    device->close   = FM_NULL;
    device->read    = i2c_bus_device_read;
    device->write   = i2c_bus_device_write;
    device->control = i2c_bus_device_control;
#endif

    /* register to device manager */
    fm_device_register(device, name, FM_DEVICE_FLAG_RDWR);

    return FM_EOK;
}
