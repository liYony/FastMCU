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

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <fmconfig.h>
#include "hc32_ll.h"
#include "drv_config.h"

/*********** UART configure *********/
#if defined(BSP_USING_UART1)
    #define USART1_RX_PORT                  (GPIO_PORT_B)
    #define USART1_RX_PIN                   (GPIO_PIN_00)

    #define USART1_TX_PORT                  (GPIO_PORT_B)
    #define USART1_TX_PIN                   (GPIO_PIN_01)
#endif

#if defined(BSP_USING_UART3)
    #define USART3_RX_PORT                  (GPIO_PORT_B)
    #define USART3_RX_PIN                   (GPIO_PIN_00)

    #define USART3_TX_PORT                  (GPIO_PORT_B)
    #define USART3_TX_PIN                   (GPIO_PIN_01)
#endif

#if defined(BSP_USING_UART6)
    #define USART6_RX_PORT                  (GPIO_PORT_E)
    #define USART6_RX_PIN                   (GPIO_PIN_08)

    #define USART6_TX_PORT                  (GPIO_PORT_E)
    #define USART6_TX_PIN                   (GPIO_PIN_09)
#endif

#if defined(BSP_USING_PWM_TMRA_2)
    #define PWM2_CH4_PORT                   (GPIO_PORT_A)
    #define PWM2_CH4_PIN                    (GPIO_PIN_03)
#endif

#if defined(BSP_USING_PWM_TMRA_12)
    #define PWM12_CH2_PORT                  (GPIO_PORT_B)
    #define PWM12_CH2_PIN                   (GPIO_PIN_04)
#endif

#if defined(BSP_USING_PWM_TMRA_10)
    #define PWM10_CH1_PORT                  (GPIO_PORT_B)
    #define PWM10_CH1_PIN                   (GPIO_PIN_06)
    #define PWM10_CH2_PORT                  (GPIO_PORT_B)
    #define PWM10_CH2_PIN                   (GPIO_PIN_07)
#endif

#if defined(BSP_USING_ADC1)
    #define ADC1_CH3_PORT                    (GPIO_PORT_A)
    #define ADC1_CH3_PIN                     (GPIO_PIN_03)
#endif

#if defined(BSP_USING_ADC2)
    #define ADC2_CH4_PORT                    (GPIO_PORT_A)
    #define ADC2_CH4_PIN                     (GPIO_PIN_04)
#endif

#if defined(BSP_USING_ADC3)
    #define ADC3_CH12_PORT                   (GPIO_PORT_C)
    #define ADC3_CH12_PIN                    (GPIO_PIN_02)
#endif

#endif
