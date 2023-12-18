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

#if defined(BSP_USING_UART6)
    #define USART6_RX_PORT                  (GPIO_PORT_E)
    #define USART6_RX_PIN                   (GPIO_PIN_08)

    #define USART6_TX_PORT                  (GPIO_PORT_E)
    #define USART6_TX_PIN                   (GPIO_PIN_09)
#endif

#endif
