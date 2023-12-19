/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 * 2018-11-16     Ernest Chen  add finsh command and update adc function
 * 2022-05-11     Stanley Lwin add finsh voltage conversion command
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#ifndef __ADC_H__
#define __ADC_H__

#include <fmcu.h>

#define FM_ADC_INTERN_CH_TEMPER     (-1)
#define FM_ADC_INTERN_CH_VREF       (-2)
#define FM_ADC_INTERN_CH_VBAT       (-3)

struct fm_adc_device;
struct fm_adc_ops
{
    fm_err_t (*enabled)(struct fm_adc_device *device, fm_int8_t channel, fm_bool_t enabled);
    fm_err_t (*convert)(struct fm_adc_device *device, fm_int8_t channel, fm_uint32_t *value);
    fm_uint8_t (*get_resolution)(struct fm_adc_device *device);
    fm_int16_t (*get_vref) (struct fm_adc_device *device);
};

struct fm_adc_device
{
    struct fm_device parent;
    const struct fm_adc_ops *ops;
};
typedef struct fm_adc_device *fm_adc_device_t;

typedef enum
{
    FM_ADC_CMD_ENABLE = FM_DEVICE_CTRL_BASE(ADC) + 1,
    FM_ADC_CMD_DISABLE = FM_DEVICE_CTRL_BASE(ADC) + 2,
    FM_ADC_CMD_GET_RESOLUTION = FM_DEVICE_CTRL_BASE(ADC) + 3,   /* get the resolution in bits */
    FM_ADC_CMD_GET_VREF = FM_DEVICE_CTRL_BASE(ADC) + 4,         /* get reference voltage */
} fm_adc_cmd_t;

fm_err_t fm_hw_adc_register(fm_adc_device_t adc,const char *name, const struct fm_adc_ops *ops, const void *user_data);
fm_uint32_t fm_adc_read(fm_adc_device_t dev, fm_int8_t channel);
fm_err_t fm_adc_enable(fm_adc_device_t dev, fm_int8_t channel);
fm_err_t fm_adc_disable(fm_adc_device_t dev, fm_int8_t channel);
fm_int16_t fm_adc_voltage(fm_adc_device_t dev, fm_int8_t channel);

#endif /* __ADC_H__ */
