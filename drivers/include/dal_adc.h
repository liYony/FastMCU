#ifndef __DAL_ADC_H__
#define __DAL_ADC_H__

#include <dal_type.h>

typedef enum
{
    DAL_ADC_0,
    DAL_ADC_1,
    DAL_ADC_2,
    DAL_ADC_NUMBER
} dal_adc_number_t;

typedef enum
{
    DAL_ADC_CH0,
    DAL_ADC_CH1,
    DAL_ADC_CH2,
    DAL_ADC_CH3,
    DAL_ADC_CH4,
    DAL_ADC_CH5,
    DAL_ADC_CH6,
    DAL_ADC_CH7,
    DAL_ADC_CH8,
    DAL_ADC_CH9,
    DAL_ADC_CH10,
    DAL_ADC_CH11,
    DAL_ADC_CH12,
    DAL_ADC_CH13,
    DAL_ADC_CH14,
    DAL_ADC_CH15,
    DAL_ADC_CH16,
    DAL_ADC_CH17,
    DAL_ADC_CH18,
    DAL_ADC_CH19,
    DAL_ADC_CH20,
    DAL_ADC_CHNUMBER
} dal_adc_channel_t;

int mcu_adc_init(dal_adc_number_t adc);
int mcu_adc_enable(dal_adc_number_t adc, dal_adc_channel_t ch);
int mcu_adc_disable(dal_adc_number_t adc, dal_adc_channel_t ch);
uint32_t mcu_adc_read(dal_adc_number_t adc, dal_adc_channel_t ch);

int dal_adc_init(dal_adc_number_t adc);
int dal_adc_enable(dal_adc_number_t adc, dal_adc_channel_t ch);
int dal_adc_disable(dal_adc_number_t adc, dal_adc_channel_t ch);
uint32_t dal_adc_read(dal_adc_number_t adc, dal_adc_channel_t ch);

#endif /* __DAL_ADC_H__ */
