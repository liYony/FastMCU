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

#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#include <fmcu.h>
#include "irq_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BSP_USING_UART3)
#ifndef UART3_CONFIG
#define UART3_CONFIG                                            \
    {                                                           \
        .name     = "uart3",                                    \
        .Instance = CM_USART3,                                  \
        .clock    = FCG3_PERIPH_USART3,                         \
        .rxerr_irq.irq_config =                                 \
        {                                                       \
            .irq_num    = BSP_UART3_RXERR_IRQ_NUM,              \
            .irq_prio   = BSP_UART3_RXERR_IRQ_PRIO,             \
            .int_src    = INT_SRC_USART3_EI,                    \
        },                                                      \
        .rx_irq.irq_config =                                    \
        {                                                       \
            .irq_num    = BSP_UART3_RX_IRQ_NUM,                 \
            .irq_prio   = BSP_UART3_RX_IRQ_PRIO,                \
            .int_src    = INT_SRC_USART3_RI,                    \
        },                                                      \
        .tx_irq.irq_config =                                    \
        {                                                       \
            .irq_num    = BSP_UART3_TX_IRQ_NUM,                 \
            .irq_prio   = BSP_UART3_TX_IRQ_PRIO,                \
            .int_src    = INT_SRC_USART3_TI,                    \
        },                                                      \
        .tx_cplt_irq.irq_config =                               \
        {                                                       \
            .irq_num    = BSP_UART3_TX_CPLT_IRQ_NUM,            \
            .irq_prio   = BSP_UART3_TX_CPLT_IRQ_PRIO,           \
            .int_src    = INT_SRC_USART3_TCI,                   \
        },                                                      \
    }
#endif /* UART3_CONFIG */
#endif /* BSP_USING_UART3 */

#ifdef __cplusplus
}
#endif

#endif
