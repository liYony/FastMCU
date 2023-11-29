#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <fmconfig.h>
#include "hc32_ll.h"

#if defined(DAL_SPI_ENABLE)
/* SCK = PE03 */
#define SPI4_SCK_PORT                    (GPIO_PORT_E)
#define SPI4_SCK_PIN                     (GPIO_PIN_03)
/* MOSI = PE4 */
#define SPI4_MOSI_PORT                   (GPIO_PORT_E)
#define SPI4_MOSI_PIN                    (GPIO_PIN_04)
/* MISO = PE2 */
#define SPI4_MISO_PORT                   (GPIO_PORT_E)
#define SPI4_MISO_PIN                    (GPIO_PIN_02)

int hc32_borad_spi_init(CM_SPI_TypeDef *SPIx);
#endif /* DAL_SPI_ENABLE */

#if defined(DAL_PWM_ENABLE)
#define PWM2_CH4_PORT                    (GPIO_PORT_A)
#define PWM2_CH4_PIN                     (GPIO_PIN_03)

#define PWM12_CH2_PORT                   (GPIO_PORT_B)
#define PWM12_CH2_PIN                    (GPIO_PIN_04)

#define PWM10_CH2_PORT                   (GPIO_PORT_B)
#define PWM10_CH2_PIN                    (GPIO_PIN_07)

int hc32_borad_pwm_init(CM_TMRA_TypeDef *PWMx);
#endif

#if defined(DAL_ADC_ENABLE)
#define ADC2_CH3_PORT                    (GPIO_PORT_A)
#define ADC2_CH3_PIN                     (GPIO_PIN_03)

#define ADC2_CH6_PORT                    (GPIO_PORT_A)
#define ADC2_CH6_PIN                     (GPIO_PIN_06)

int hc32_borad_adc_init(CM_ADC_TypeDef *ADCx);
#endif

#define GPIO_IRQN_BASE (INT010_IRQn)

struct hc32_irq_config
{
    IRQn_Type       irq_num;
    uint32_t        irq_prio;
    en_int_src_t    int_src;
};

int hc32_install_irq_handler(struct hc32_irq_config *irq_config,
                                  void (*irq_hdr)(void));

#endif
