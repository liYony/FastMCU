#include "board_config.h"

int hc32_install_irq_handler(struct hc32_irq_config *irq_config,
                                  void (*irq_hdr)(void))
{
    int result = -1;

    if (irq_config == NULL || irq_hdr == NULL)
    {
        return result;
    }
    
    stc_irq_signin_config_t stcIrqSignConfig;
    stcIrqSignConfig.enIRQn      = irq_config->irq_num;
    stcIrqSignConfig.enIntSrc    = irq_config->int_src;
    stcIrqSignConfig.pfnCallback = irq_hdr;
    if (LL_OK == INTC_IrqSignIn(&stcIrqSignConfig))
    {
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, irq_config->irq_prio);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
        result = 0;
    }
    return result;
}

#if defined(DAL_SPI_ENABLE)
int hc32_borad_spi_init(CM_SPI_TypeDef *SPIx)
{
    int result = 0;

    switch ((uint32_t)SPIx)
    {
        case (uint32_t)CM_SPI4:
            /* Configure SPI SCK/MOSI/MISO pin. */
            GPIO_SetFunc(SPI4_SCK_PORT,  SPI4_SCK_PIN,  GPIO_FUNC_40);
            GPIO_SetFunc(SPI4_MOSI_PORT, SPI4_MOSI_PIN, GPIO_FUNC_41);
            GPIO_SetFunc(SPI4_MISO_PORT, SPI4_MISO_PIN, GPIO_FUNC_42);
            break;
        default:
            result = -1;
            break;
    }

    return result;
}
#endif

#if defined(DAL_PWM_ENABLE)
int hc32_borad_pwm_init(CM_TMRA_TypeDef *PWMx)
{
    int result = 0;
    switch ((uint32_t)PWMx)
    {
        case (uint32_t)CM_TMRA_2:
            /* Configure CH pin. */
            GPIO_SetFunc(PWM2_CH4_PORT, PWM2_CH4_PIN,  GPIO_FUNC_4);
            break;
        case (uint32_t)CM_TMRA_12:
            /* Configure CH pin. */
            GPIO_SetFunc(PWM12_CH2_PORT, PWM12_CH2_PIN,  GPIO_FUNC_5);
            break;
        case (uint32_t)CM_TMRA_10:
            /* Configure CH pin. */
            GPIO_SetFunc(PWM10_CH2_PORT, PWM10_CH2_PIN,  GPIO_FUNC_6);
            break;
        default:
            result = -1;
            break;
    }
    return result;
}
#endif

#if defined(DAL_ADC_ENABLE)
int hc32_borad_adc_init(CM_ADC_TypeDef *ADCx)
{
    int result = 0;
    stc_gpio_init_t stcGpioInit;
    
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    switch ((uint32_t)ADCx)
    {
        case (uint32_t)CM_ADC2:
            /* Configure CH pin. */
            (void)GPIO_Init(ADC2_CH3_PORT, ADC2_CH3_PIN, &stcGpioInit);
            (void)GPIO_Init(ADC2_CH6_PORT, ADC2_CH6_PIN, &stcGpioInit);
            break;
        default:
            result = -1;
            break;
    }
    return result;
}
#endif
