/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 * 2021-02-06     Meco Man     fix FM_ENOSYS code in negative
 * 2022-04-29     WangQiang    add pin operate command in MSH
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#include <fmcu.h>
#include <fmdevice.h>

static struct fm_device_pin _hw_pin;

static fm_ssize_t _pin_read(fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size)
{
    struct fm_device_pin_value *value;
    struct fm_device_pin *pin = (struct fm_device_pin *)dev;

    /* check parameters */
    FM_ASSERT(pin != FM_NULL);

    value = (struct fm_device_pin_value *)buffer;
    if (value == FM_NULL || size != sizeof(*value))
        return 0;

    value->value = pin->ops->pin_read(dev, value->pin);
    return size;
}

static fm_ssize_t _pin_write(fm_device_t dev, fm_off_t pos, const void *buffer, fm_size_t size)
{
    struct fm_device_pin_value *value;
    struct fm_device_pin *pin = (struct fm_device_pin *)dev;

    /* check parameters */
    FM_ASSERT(pin != FM_NULL);

    value = (struct fm_device_pin_value *)buffer;
    if (value == FM_NULL || size != sizeof(*value))
        return 0;

    pin->ops->pin_write(dev, (fm_base_t)value->pin, (fm_base_t)value->value);

    return size;
}

static fm_err_t _pin_control(fm_device_t dev, int cmd, void *args)
{
    struct fm_device_pin_mode *mode;
    struct fm_device_pin *pin = (struct fm_device_pin *)dev;

    /* check parameters */
    FM_ASSERT(pin != FM_NULL);

    mode = (struct fm_device_pin_mode *)args;
    if (mode == FM_NULL)
        return -FM_ERROR;

    pin->ops->pin_mode(dev, (fm_base_t)mode->pin, (fm_base_t)mode->mode);

    return 0;
}

#ifdef FM_USING_DEVICE_OPS
const static struct fm_device_ops pin_ops =
{
    FM_NULL,
    FM_NULL,
    FM_NULL,
    _pin_read,
    _pin_write,
    _pin_control
};
#endif

int fm_device_pin_register(const char *name, const struct fm_pin_ops *ops, void *user_data)
{
    _hw_pin.parent.type         = FM_Device_Class_Pin;
    _hw_pin.parent.rx_indicate  = FM_NULL;
    _hw_pin.parent.tx_complete  = FM_NULL;

#ifdef FM_USING_DEVICE_OPS
    _hw_pin.parent.ops          = &pin_ops;
#else
    _hw_pin.parent.init         = FM_NULL;
    _hw_pin.parent.open         = FM_NULL;
    _hw_pin.parent.close        = FM_NULL;
    _hw_pin.parent.read         = _pin_read;
    _hw_pin.parent.write        = _pin_write;
    _hw_pin.parent.control      = _pin_control;
#endif

    _hw_pin.ops                 = ops;
    _hw_pin.parent.user_data    = user_data;

    /* register a character device */
    fm_device_register(&_hw_pin.parent, name, FM_DEVICE_FLAG_RDWR);

    return 0;
}

fm_err_t fm_pin_attach_irq(fm_base_t pin, fm_uint8_t mode,
                           void (*hdr)(void *args), void *args)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);
    if (_hw_pin.ops->pin_attach_irq)
    {
        return _hw_pin.ops->pin_attach_irq(&_hw_pin.parent, pin, mode, hdr, args);
    }
    return -FM_ENOSYS;
}

fm_err_t fm_pin_detach_irq(fm_base_t pin)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);
    if (_hw_pin.ops->pin_detach_irq)
    {
        return _hw_pin.ops->pin_detach_irq(&_hw_pin.parent, pin);
    }
    return -FM_ENOSYS;
}

fm_err_t fm_pin_irq_enable(fm_base_t pin, fm_uint8_t enabled)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);
    if (_hw_pin.ops->pin_irq_enable)
    {
        return _hw_pin.ops->pin_irq_enable(&_hw_pin.parent, pin, enabled);
    }
    return -FM_ENOSYS;
}

/* RT-Thread Hardware PIN APIs */
void fm_pin_mode(fm_base_t pin, fm_uint8_t mode)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);
    _hw_pin.ops->pin_mode(&_hw_pin.parent, pin, mode);
}

void fm_pin_write(fm_base_t pin, fm_uint8_t value)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);
    _hw_pin.ops->pin_write(&_hw_pin.parent, pin, value);
}

fm_int8_t fm_pin_read(fm_base_t pin)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);
    return _hw_pin.ops->pin_read(&_hw_pin.parent, pin);
}

/* Get pin number by name, such as PA.0, P0.12 */
fm_base_t fm_pin_get(const char *name)
{
    FM_ASSERT(_hw_pin.ops != FM_NULL);

    if (_hw_pin.ops->pin_get == FM_NULL)
    {
        return -FM_ENOSYS;
    }
    return _hw_pin.ops->pin_get(name);
}
