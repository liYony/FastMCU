/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 */

#include <fmdevice.h>
#include "board_config.h"

/**
 * The below functions will initialize HC32 board.
 */

#if defined FM_USING_SERIAL
fm_err_t fm_hw_board_uart_init(CM_USART_TypeDef *USARTx)
{
    fm_err_t result = FM_EOK;

    switch ((fm_uint32_t)USARTx)
    {
#if defined(BSP_USING_UART1)
    case (fm_uint32_t)CM_USART1:
        /* Configure USART RX/TX pin. */
        GPIO_SetFunc(USART1_RX_PORT, USART1_RX_PIN, GPIO_FUNC_33);
        GPIO_SetFunc(USART1_TX_PORT, USART1_TX_PIN, GPIO_FUNC_32);
        break;
#endif
#if defined(BSP_USING_UART3)
    case (fm_uint32_t)CM_USART3:
        /* Configure USART RX/TX pin. */
        GPIO_SetFunc(USART3_RX_PORT, USART3_RX_PIN, GPIO_FUNC_37);
        GPIO_SetFunc(USART3_TX_PORT, USART3_TX_PIN, GPIO_FUNC_36);
        break;
#endif
#if defined(BSP_USING_UART6)
    case (fm_uint32_t)CM_USART6:
        /* Configure USART RX/TX pin. */
        GPIO_SetFunc(USART6_RX_PORT, USART6_RX_PIN, GPIO_FUNC_37);
        GPIO_SetFunc(USART6_TX_PORT, USART6_TX_PIN, GPIO_FUNC_36);
        break;
#endif
    default:
        result = -FM_ERROR;
        break;
    }

    return result;
}
#endif

#if defined FM_USING_PWM
int fm_hw_board_pwm_tmra_init(CM_TMRA_TypeDef *PWMx)
{
    int result = 0;
    switch ((uint32_t)PWMx)
    {
#if defined(BSP_USING_PWM_TMRA_2)
        case (uint32_t)CM_TMRA_2:
            /* Configure CH pin. */
            GPIO_SetFunc(PWM2_CH4_PORT, PWM2_CH4_PIN,  GPIO_FUNC_4);
            break;
#endif
#if defined(BSP_USING_PWM_TMRA_12)
        case (uint32_t)CM_TMRA_12:
            /* Configure CH pin. */
            GPIO_SetFunc(PWM12_CH2_PORT, PWM12_CH2_PIN,  GPIO_FUNC_5);
            break;
#endif
#if defined(BSP_USING_PWM_TMRA_10)
        case (uint32_t)CM_TMRA_10:
            /* Configure CH pin. */
            GPIO_SetFunc(PWM10_CH1_PORT, PWM10_CH1_PIN,  GPIO_FUNC_6);
            GPIO_SetFunc(PWM10_CH2_PORT, PWM10_CH2_PIN,  GPIO_FUNC_6);
            break;
#endif
        default:
            result = -FM_ERROR;
            break;
    }
    return result;
}
#endif

#if defined FM_USING_ADC
fm_err_t fm_hw_board_adc_init(CM_ADC_TypeDef *ADCx)
{
    fm_err_t result = FM_EOK;
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    switch ((fm_uint32_t)ADCx)
    {
#if defined(BSP_USING_ADC1)
    case (fm_uint32_t)CM_ADC1:
        (void)GPIO_Init(ADC1_CH3_PORT, ADC1_CH3_PIN, &stcGpioInit);
        break;
#endif
#if defined(BSP_USING_ADC2)
    case (fm_uint32_t)CM_ADC2:
        (void)GPIO_Init(ADC2_CH4_PORT, ADC2_CH4_PIN, &stcGpioInit);
        break;
#endif
#if defined(BSP_USING_ADC3)
    case (fm_uint32_t)CM_ADC3:
        (void)GPIO_Init(ADC3_CH12_PORT, ADC3_CH12_PIN, &stcGpioInit);
        break;
#endif
    default:
        result = -FM_ERROR;
        break;
    }

    return result;
}
#endif
