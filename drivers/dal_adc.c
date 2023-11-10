#include <dal_adc.h>

dal_weak int mcu_adc_init(dal_adc_number_t adc)
{
    return -1;
}

dal_weak int mcu_adc_enable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return -1;
}

dal_weak int mcu_adc_disable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return -1;
}

dal_weak uint32_t mcu_adc_read(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return (uint32_t)-1;
}

int dal_adc_init(dal_adc_number_t adc)
{
    return mcu_adc_init(adc);
}

int dal_adc_enable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return mcu_adc_enable(adc, ch);
}

int dal_adc_disable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return mcu_adc_disable(adc, ch);
}

uint32_t dal_adc_read(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return mcu_adc_read(adc, ch);
}
