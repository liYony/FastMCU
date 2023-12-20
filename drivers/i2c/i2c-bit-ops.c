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

#include <fmdevice.h>

#define DBG_TAG               "I2C"
#ifdef FM_I2C_BITOPS_DEBUG
#define DBG_LVL               DBG_LOG
#else
#define DBG_LVL               DBG_INFO
#endif
#include <fmdbg.h>

#define SET_SDA(ops, val)   ops->set_sda(ops->data, val)
#define SET_SCL(ops, val)   ops->set_scl(ops->data, val)
#define GET_SDA(ops)        ops->get_sda(ops->data)
#define GET_SCL(ops)        ops->get_scl(ops->data)

fm_inline void i2c_delay(struct fm_i2c_bit_ops *ops)
{
    ops->udelay((ops->delay_us + 1) >> 1);
}

fm_inline void i2c_delay2(struct fm_i2c_bit_ops *ops)
{
    ops->udelay(ops->delay_us);
}

#define SDA_L(ops)          SET_SDA(ops, 0)
#define SDA_H(ops)          SET_SDA(ops, 1)
#define SCL_L(ops)          SET_SCL(ops, 0)

/**
 * release scl line, and wait scl line to high.
 */
static fm_err_t SCL_H(struct fm_i2c_bit_ops *ops)
{
    fm_tick_t start;

    SET_SCL(ops, 1);

    if (!ops->get_scl)
        goto done;

    start = fm_get_systick();
    while (!GET_SCL(ops))
    {
        if ((fm_get_systick() - start) > ops->timeout)
            return -FM_ETIMEOUT;
        i2c_delay(ops);
    }
#ifdef FM_I2C_BITOPS_DEBUG
    if (fm_tick_get() != start)
    {
        LOG_D("wait %ld tick for SCL line to go high",
              fm_tick_get() - start);
    }
#endif

done:
    i2c_delay(ops);

    return FM_EOK;
}

static void i2c_start(struct fm_i2c_bit_ops *ops)
{
#ifdef FM_I2C_BITOPS_DEBUG
    if (ops->get_scl && !GET_SCL(ops))
    {
        LOG_E("I2C bus error, SCL line low");
    }
    if (ops->get_sda && !GET_SDA(ops))
    {
        LOG_E("I2C bus error, SDA line low");
    }
#endif
    SDA_L(ops);
    i2c_delay(ops);
    SCL_L(ops);
}

static void i2c_restart(struct fm_i2c_bit_ops *ops)
{
    SDA_H(ops);
    SCL_H(ops);
    i2c_delay(ops);
    SDA_L(ops);
    i2c_delay(ops);
    SCL_L(ops);
}

static void i2c_stop(struct fm_i2c_bit_ops *ops)
{
    SDA_L(ops);
    i2c_delay(ops);
    SCL_H(ops);
    i2c_delay(ops);
    SDA_H(ops);
    i2c_delay2(ops);
}

fm_inline fm_bool_t i2c_waitack(struct fm_i2c_bit_ops *ops)
{
    fm_bool_t ack;

    SDA_H(ops);
    i2c_delay(ops);

    if (SCL_H(ops) < 0)
    {
        LOG_W("wait ack timeout");

        return -FM_ETIMEOUT;
    }

    ack = !GET_SDA(ops);    /* ACK : SDA pin is pulled low */
    LOG_D("%s", ack ? "ACK" : "NACK");

    SCL_L(ops);

    return ack;
}

static fm_int32_t i2c_writeb(struct fm_i2c_bus_device *bus, fm_uint8_t data)
{
    fm_int32_t i;
    fm_uint8_t bit;

    struct fm_i2c_bit_ops *ops = (struct fm_i2c_bit_ops *)bus->priv;

    for (i = 7; i >= 0; i--)
    {
        SCL_L(ops);
        bit = (data >> i) & 1;
        SET_SDA(ops, bit);
        i2c_delay(ops);
        if (SCL_H(ops) < 0)
        {
            LOG_D("i2c_writeb: 0x%02x, "
                    "wait scl pin high timeout at bit %d",
                    data, i);

            return -FM_ETIMEOUT;
        }
    }
    SCL_L(ops);
    i2c_delay(ops);

    return i2c_waitack(ops);
}

static fm_int32_t i2c_readb(struct fm_i2c_bus_device *bus)
{
    fm_uint8_t i;
    fm_uint8_t data = 0;
    struct fm_i2c_bit_ops *ops = (struct fm_i2c_bit_ops *)bus->priv;

    SDA_H(ops);
    i2c_delay(ops);
    for (i = 0; i < 8; i++)
    {
        data <<= 1;

        if (SCL_H(ops) < 0)
        {
            LOG_D("i2c_readb: wait scl pin high "
                    "timeout at bit %d", 7 - i);

            return -FM_ETIMEOUT;
        }

        if (GET_SDA(ops))
            data |= 1;
        SCL_L(ops);
        i2c_delay2(ops);
    }

    return data;
}

static fm_ssize_t i2c_send_bytes(struct fm_i2c_bus_device *bus,
                                struct fm_i2c_msg        *msg)
{
    fm_int32_t ret;
    fm_size_t bytes = 0;
    const fm_uint8_t *ptr = msg->buf;
    fm_int32_t count = msg->len;
    fm_uint16_t ignore_nack = msg->flags & FM_I2C_IGNORE_NACK;

    while (count > 0)
    {
        ret = i2c_writeb(bus, *ptr);

        if ((ret > 0) || (ignore_nack && (ret == 0)))
        {
            count --;
            ptr ++;
            bytes ++;
        }
        else if (ret == 0)
        {
            LOG_D("send bytes: NACK.");

            return 0;
        }
        else
        {
            LOG_E("send bytes: error %d", ret);

            return ret;
        }
    }

    return bytes;
}

static fm_err_t i2c_send_ack_or_nack(struct fm_i2c_bus_device *bus, int ack)
{
    struct fm_i2c_bit_ops *ops = (struct fm_i2c_bit_ops *)bus->priv;

    if (ack)
        SET_SDA(ops, 0);
    i2c_delay(ops);
    if (SCL_H(ops) < 0)
    {
        LOG_E("ACK or NACK timeout.");

        return -FM_ETIMEOUT;
    }
    SCL_L(ops);

    return FM_EOK;
}

static fm_ssize_t i2c_recv_bytes(struct fm_i2c_bus_device *bus,
                                struct fm_i2c_msg        *msg)
{
    fm_int32_t val;
    fm_int32_t bytes = 0;   /* actual bytes */
    fm_uint8_t *ptr = msg->buf;
    fm_int32_t count = msg->len;
    const fm_uint32_t flags = msg->flags;

    while (count > 0)
    {
        val = i2c_readb(bus);
        if (val >= 0)
        {
            *ptr = val;
            bytes ++;
        }
        else
        {
            break;
        }

        ptr ++;
        count --;

        LOG_D("recieve bytes: 0x%02x, %s",
                val, (flags & FM_I2C_NO_READ_ACK) ?
                "(No ACK/NACK)" : (count ? "ACK" : "NACK"));

        if (!(flags & FM_I2C_NO_READ_ACK))
        {
            val = i2c_send_ack_or_nack(bus, count);
            if (val < 0)
                return val;
        }
    }

    return bytes;
}

static fm_int32_t i2c_send_address(struct fm_i2c_bus_device *bus,
                                   fm_uint8_t                addr,
                                   fm_int32_t                retries)
{
    struct fm_i2c_bit_ops *ops = (struct fm_i2c_bit_ops *)bus->priv;
    fm_int32_t i;
    fm_err_t ret = 0;

    for (i = 0; i <= retries; i++)
    {
        ret = i2c_writeb(bus, addr);
        if (ret == 1 || i == retries)
            break;
        LOG_D("send stop condition");
        i2c_stop(ops);
        i2c_delay2(ops);
        LOG_D("send start condition");
        i2c_start(ops);
    }

    return ret;
}

static fm_err_t i2c_bit_send_address(struct fm_i2c_bus_device *bus,
                                     struct fm_i2c_msg        *msg)
{
    fm_uint16_t flags = msg->flags;
    fm_uint16_t ignore_nack = msg->flags & FM_I2C_IGNORE_NACK;
    struct fm_i2c_bit_ops *ops = (struct fm_i2c_bit_ops *)bus->priv;

    fm_uint8_t addr1, addr2;
    fm_int32_t retries;
    fm_err_t ret;

    retries = ignore_nack ? 0 : bus->retries;

    if (flags & FM_I2C_ADDR_10BIT)
    {
        addr1 = 0xf0 | ((msg->addr >> 7) & 0x06);
        addr2 = msg->addr & 0xff;

        LOG_D("addr1: %d, addr2: %d", addr1, addr2);

        ret = i2c_send_address(bus, addr1, retries);
        if ((ret != 1) && !ignore_nack)
        {
            LOG_W("NACK: sending first addr");

            return -FM_EIO;
        }

        ret = i2c_writeb(bus, addr2);
        if ((ret != 1) && !ignore_nack)
        {
            LOG_W("NACK: sending second addr");

            return -FM_EIO;
        }
        if (flags & FM_I2C_RD)
        {
            LOG_D("send repeated start condition");
            i2c_restart(ops);
            addr1 |= 0x01;
            ret = i2c_send_address(bus, addr1, retries);
            if ((ret != 1) && !ignore_nack)
            {
                LOG_E("NACK: sending repeated addr");

                return -FM_EIO;
            }
        }
    }
    else
    {
        /* 7-bit addr */
        addr1 = msg->addr << 1;
        if (flags & FM_I2C_RD)
            addr1 |= 1;
        ret = i2c_send_address(bus, addr1, retries);
        if ((ret != 1) && !ignore_nack)
            return -FM_EIO;
    }

    return FM_EOK;
}

static fm_ssize_t i2c_bit_xfer(struct fm_i2c_bus_device *bus,
                              struct fm_i2c_msg         msgs[],
                              fm_uint32_t               num)
{
    struct fm_i2c_msg *msg;
    struct fm_i2c_bit_ops *ops = (struct fm_i2c_bit_ops *)bus->priv;
    fm_int32_t ret;
    fm_uint32_t i;
    fm_uint16_t ignore_nack;

    if (num == 0) return 0;

    for (i = 0; i < num; i++)
    {
        msg = &msgs[i];
        ignore_nack = msg->flags & FM_I2C_IGNORE_NACK;
        if (!(msg->flags & FM_I2C_NO_START))
        {
            if (i)
            {
                i2c_restart(ops);
            }
            else
            {
                LOG_D("send start condition");
                i2c_start(ops);
            }
            ret = i2c_bit_send_address(bus, msg);
            if ((ret != FM_EOK) && !ignore_nack)
            {
                LOG_D("receive NACK from device addr 0x%02x msg %d",
                        msgs[i].addr, i);
                goto out;
            }
        }
        if (msg->flags & FM_I2C_RD)
        {
            ret = i2c_recv_bytes(bus, msg);
            if (ret >= 1)
            {
                LOG_D("read %d byte%s", ret, ret == 1 ? "" : "s");
            }
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -FM_EIO;
                goto out;
            }
        }
        else
        {
            ret = i2c_send_bytes(bus, msg);
            if (ret >= 1)
            {
                LOG_D("write %d byte%s", ret, ret == 1 ? "" : "s");
            }
            if (ret < msg->len)
            {
                if (ret >= 0)
                    ret = -FM_ERROR;
                goto out;
            }
        }
    }
    ret = i;

out:
    if (!(msg->flags & FM_I2C_NO_STOP))
    {
        LOG_D("send stop condition");
        i2c_stop(ops);
    }

    return ret;
}

static const struct fm_i2c_bus_device_ops i2c_bit_bus_ops =
{
    i2c_bit_xfer,
    FM_NULL,
    FM_NULL
};

fm_err_t fm_i2c_bit_add_bus(struct fm_i2c_bus_device *bus,
                            const char               *bus_name)
{
    bus->ops = &i2c_bit_bus_ops;

    return fm_i2c_bus_device_register(bus, bus_name);
}
