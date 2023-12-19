/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-19     liYony       first version
 */

#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "drv_config.h"

struct stm32_adc
{
    ADC_HandleTypeDef ADC_Handler;
    struct fm_adc_device stm32_adc_device;
};

#endif
