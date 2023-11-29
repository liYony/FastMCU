#include <dal.h>
#include "hc32_ll.h"
#include "board_config.h"

typedef struct
{
    dal_adc_number_t adc;
    CM_ADC_TypeDef *unit_adc;
    uint32_t fcg_clk;
    uint8_t seq;
} adc_info_t;

static adc_info_t adc_info[] = 
{
    {DAL_ADC_0, CM_ADC1,  FCG3_PERIPH_ADC1, ADC_SEQ_A},
    {DAL_ADC_1, CM_ADC2,  FCG3_PERIPH_ADC2, ADC_SEQ_A},
};

static void hc32_adc_init(adc_info_t *info)
{
    stc_adc_init_t stcAdcInit;

    FCG_Fcg3PeriphClockCmd(info->fcg_clk, ENABLE);

    (void)ADC_StructInit(&stcAdcInit);
    (void)ADC_Init(info->unit_adc, &stcAdcInit);

    hc32_borad_adc_init(info->unit_adc);
    
    ADC_DataRegAutoClearCmd(info->unit_adc,ENABLE);
}

static int hc32_adc_get_channel(dal_adc_channel_t channel, uint8_t *hc32_channel)
{
    *hc32_channel = channel;
    if (channel == DAL_ADC_CHVREF)
    {
        *hc32_channel = ADC_EXT_CH;
    }
    return 0;
}

int mcu_adc_init(dal_adc_number_t adc)
{
    hc32_adc_init(&adc_info[adc]);
    return 0;
}

int mcu_adc_enable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    uint8_t hc32_ch;
    
    hc32_adc_get_channel(ch, &hc32_ch);
    
    if (hc32_ch == ADC_EXT_CH)
    {
        ADC_ConvDataAverageConfig(adc_info[adc].unit_adc, ADC_AVG_CNT256);
        ADC_ConvDataAverageChCmd(adc_info[adc].unit_adc, hc32_ch, ENABLE);

        PWC_PowerMonitorCmd(ENABLE);
        ADC_SetExtChSrc(adc_info[adc].unit_adc, ADC_EXTCH_INTERN_ANALOG_SRC);
    }
    
    ADC_ChCmd(adc_info[adc].unit_adc, adc_info[adc].seq, (uint8_t)hc32_ch, ENABLE);
    ADC_Start(adc_info[adc].unit_adc);
    return 0;
}

int mcu_adc_disable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    uint8_t hc32_ch;
    
    hc32_adc_get_channel(ch, &hc32_ch);
    
    ADC_ChCmd(adc_info[adc].unit_adc, adc_info[adc].seq, hc32_ch, DISABLE);
    ADC_Stop(adc_info[adc].unit_adc);
    return 0;
}

uint32_t mcu_adc_read(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    en_flag_status_t result = RESET;
    uint32_t adc_value;
    
    uint32_t start_time = dal_get_systick();
    do
    {
        if (ADC_GetStatus(adc_info[adc].unit_adc, ADC_FLAG_EOCA) == SET)
        {
            ADC_ClearStatus(adc_info[adc].unit_adc, ADC_FLAG_EOCA);
            result = SET;
            break;
        }
    }
    while ((dal_get_systick() - start_time) < 100);

    if (result == SET)
    {
        uint8_t hc32_ch;
    
        hc32_adc_get_channel(ch, &hc32_ch);
        /* Get any ADC value of sequence A channel that needed. */
        adc_value = ADC_GetValue(adc_info[adc].unit_adc, hc32_ch);
    }
    
    return adc_value;
}
