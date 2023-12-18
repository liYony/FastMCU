/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 * 2018-11-16     Ernest Chen  add finsh command and update adc function
 * 2022-05-11     Stanley Lwin add finsh voltage conversion command
 */

#include <fmcu.h>
#include <fmdevice.h>

#define DBG_TAG "adc"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

static fm_ssize_t _adc_read(fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size)
{
    fm_err_t result = FM_EOK;
    fm_size_t i;
    struct fm_adc_device *adc = (struct fm_adc_device *)dev;
    fm_uint32_t *value = (fm_uint32_t *)buffer;

    for (i = 0; i < size; i += sizeof(int))
    {
        result = adc->ops->convert(adc, pos + i, value);
        if (result != FM_EOK)
        {
            return 0;
        }
        value++;
    }

    return i;
}

static fm_err_t _adc_control(fm_device_t dev, int cmd, void *args)
{
    fm_err_t result = -FM_EINVAL;
    fm_adc_device_t adc = (struct fm_adc_device *)dev;

    if (cmd == FM_ADC_CMD_ENABLE && adc->ops->enabled)
    {
        result = adc->ops->enabled(adc, (fm_uint32_t)args, FM_TRUE);
    }
    else if (cmd == FM_ADC_CMD_DISABLE && adc->ops->enabled)
    {
        result = adc->ops->enabled(adc, (fm_uint32_t)args, FM_FALSE);
    }
    else if (cmd == FM_ADC_CMD_GET_RESOLUTION && adc->ops->get_resolution && args)
    {
        fm_uint8_t resolution = adc->ops->get_resolution(adc);
        if(resolution != 0)
        {
            *((fm_uint8_t*)args) = resolution;
            LOG_D("resolution: %d bits", resolution);
            result = FM_EOK;
        }
    }
    else if (cmd == FM_ADC_CMD_GET_VREF && adc->ops->get_vref && args)
    {
        fm_int16_t value = adc->ops->get_vref(adc);
        if(value != 0)
        {
            *((fm_int16_t *) args) = value;
            result = FM_EOK;
        }
    }

    return result;
}

#ifdef FM_USING_DEVICE_OPS
const static struct fm_device_ops adc_ops =
{
    FM_NULL,
    FM_NULL,
    FM_NULL,
    _adc_read,
    FM_NULL,
    _adc_control,
};
#endif

fm_err_t fm_hw_adc_register(fm_adc_device_t device, const char *name, const struct fm_adc_ops *ops, const void *user_data)
{
    fm_err_t result = FM_EOK;
    FM_ASSERT(ops != FM_NULL && ops->convert != FM_NULL);

    device->parent.type = FM_Device_Class_ADC;
    device->parent.rx_indicate = FM_NULL;
    device->parent.tx_complete = FM_NULL;

#ifdef FM_USING_DEVICE_OPS
    device->parent.ops         = &adc_ops;
#else
    device->parent.init        = FM_NULL;
    device->parent.open        = FM_NULL;
    device->parent.close       = FM_NULL;
    device->parent.read        = _adc_read;
    device->parent.write       = FM_NULL;
    device->parent.control     = _adc_control;
#endif
    device->ops = ops;
    device->parent.user_data = (void *)user_data;

    result = fm_device_register(&device->parent, name, FM_DEVICE_FLAG_RDWR);

    return result;
}

fm_uint32_t fm_adc_read(fm_adc_device_t dev, fm_int8_t channel)
{
    fm_uint32_t value;

    FM_ASSERT(dev);

    dev->ops->convert(dev, channel, &value);

    return value;
}

fm_err_t fm_adc_enable(fm_adc_device_t dev, fm_int8_t channel)
{
    fm_err_t result = FM_EOK;

    FM_ASSERT(dev);

    if (dev->ops->enabled != FM_NULL)
    {
        result = dev->ops->enabled(dev, channel, FM_TRUE);
    }
    else
    {
        result = -FM_ENOSYS;
    }

    return result;
}

fm_err_t fm_adc_disable(fm_adc_device_t dev, fm_int8_t channel)
{
    fm_err_t result = FM_EOK;

    FM_ASSERT(dev);

    if (dev->ops->enabled != FM_NULL)
    {
        result = dev->ops->enabled(dev, channel, FM_FALSE);
    }
    else
    {
        result = -FM_ENOSYS;
    }

    return result;
}

fm_int16_t fm_adc_voltage(fm_adc_device_t dev, fm_int8_t channel)
{
    fm_uint32_t value = 0;
    fm_int16_t vref = 0, voltage = 0;
    fm_uint8_t resolution = 0;

    FM_ASSERT(dev);

    /*get the resolution in bits*/
    if (_adc_control((fm_device_t) dev, FM_ADC_CMD_GET_RESOLUTION, &resolution) != FM_EOK)
    {
        goto _voltage_exit;
    }

    /*get the reference voltage*/
    if (_adc_control((fm_device_t) dev, FM_ADC_CMD_GET_VREF, &vref) != FM_EOK)
    {
        goto _voltage_exit;
    }

    /*read the value and convert to voltage*/
    dev->ops->convert(dev, channel, &value);
    voltage = value * vref / (1 << resolution);

_voltage_exit:
    return voltage;
}
