#include <dal.h>
#include "stm32g0xx_hal.h"

ADC_HandleTypeDef hadc1;

static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_19CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    return;
  }
}

static int stm32_adc_get_channel(dal_adc_channel_t channel, uint32_t *stm32_channel)
{
    switch ((uint8_t)channel)
    {
    case  DAL_ADC_CH0:
        *stm32_channel = ADC_CHANNEL_0;
        break;
    case  DAL_ADC_CH1:
        *stm32_channel = ADC_CHANNEL_1;
        break;
    case  DAL_ADC_CH2:
        *stm32_channel = ADC_CHANNEL_2;
        break;
    case  DAL_ADC_CH3:
        *stm32_channel = ADC_CHANNEL_3;
        break;
    case  DAL_ADC_CH4:
        *stm32_channel = ADC_CHANNEL_4;
        break;
    case  DAL_ADC_CH5:
        *stm32_channel = ADC_CHANNEL_5;
        break;
    case  DAL_ADC_CH6:
        *stm32_channel = ADC_CHANNEL_6;
        break;
    case  DAL_ADC_CH7:
        *stm32_channel = ADC_CHANNEL_7;
        break;
    case  DAL_ADC_CH8:
        *stm32_channel = ADC_CHANNEL_8;
        break;
    case  DAL_ADC_CH9:
        *stm32_channel = ADC_CHANNEL_9;
        break;
    case  DAL_ADC_CH10:
        *stm32_channel = ADC_CHANNEL_10;
        break;
    case  DAL_ADC_CH11:
        *stm32_channel = ADC_CHANNEL_11;
        break;
    case  DAL_ADC_CH12:
        *stm32_channel = ADC_CHANNEL_12;
        break;
    case  DAL_ADC_CH13:
        *stm32_channel = ADC_CHANNEL_13;
        break;
    case  DAL_ADC_CH14:
        *stm32_channel = ADC_CHANNEL_14;
        break;
    case  DAL_ADC_CH15:
        *stm32_channel = ADC_CHANNEL_15;
        break;
#ifdef ADC_CHANNEL_16
    case  DAL_ADC_CH16:
        *stm32_channel = ADC_CHANNEL_16;
        break;
#endif /* ADC_CHANNEL_16 */
    case  DAL_ADC_CH17:
        *stm32_channel = ADC_CHANNEL_17;
        break;
#ifdef ADC_CHANNEL_18
    case  DAL_ADC_CH18:
        *stm32_channel = ADC_CHANNEL_18;
        break;
#endif /* ADC_CHANNEL_18 */
#ifdef ADC_CHANNEL_19
    case  DAL_ADC_CH19:
        *stm32_channel = ADC_CHANNEL_19;
        break;
#endif /* ADC_CHANNEL_19 */
#ifdef ADC_CHANNEL_VREFINT
    case DAL_ADC_CHVREF:
        *stm32_channel = ADC_CHANNEL_VREFINT;
        break;
#endif /* ADC_CHANNEL_VREFINT */
#ifdef ADC_CHANNEL_VBAT
    case DAL_ADC_CHVBAT:
        *stm32_channel = ADC_CHANNEL_VBAT;
        break;
#endif /* ADC_CHANNEL_VBAT */
#ifdef ADC_CHANNEL_TEMPSENSOR
    case DAL_ADC_CHTEMPER:
        *stm32_channel = ADC_CHANNEL_TEMPSENSOR;
        break;
#endif /* ADC_CHANNEL_TEMPSENSOR */
    default:
        return -1;
    }

    return 0;
}

int stm32_adc_enable(dal_adc_channel_t ch)
{
    uint32_t channel;
    stm32_adc_get_channel(ch, )
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return -1;
    }
    
    HAL_ADC_Start(&hadc1);
}

uint32_t stm32_read()
{
    HAL_ADC_PollForConversion(&hadc1, 100);
    return (uint32_t)HAL_ADC_GetValue(&hadc1);
}

int mcu_adc_init(dal_adc_number_t adc)
{
    return -1;
}

int mcu_adc_enable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return -1;
}

int mcu_adc_disable(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return -1;
}

uint32_t mcu_adc_read(dal_adc_number_t adc, dal_adc_channel_t ch)
{
    return (uint32_t)-1;
}

void test(void)
{
    
}