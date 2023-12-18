#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#include "drv_config.h"

struct stm32_adc
{
    ADC_HandleTypeDef ADC_Handler;
    struct fm_adc_device stm32_adc_device;
};

#endif
