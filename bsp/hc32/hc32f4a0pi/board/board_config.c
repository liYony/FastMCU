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
            GPIO_SetFunc(PWM2_CH_PORT, PWM2_CH_PIN,  GPIO_FUNC_4);
            break;
        default:
            result = -1;
            break;
    }
    return result;
}
#endif
