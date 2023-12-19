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
        .rx_err_irq.irq_config =                                 \
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

#if defined(BSP_UART3_RX_USING_DMA)
#ifndef UART3_DMA_RX_CONFIG
#define UART3_DMA_RX_CONFIG                                     \
    {                                                           \
        .Instance       = UART3_RX_DMA_INSTANCE,                \
        .channel        = UART3_RX_DMA_CHANNEL,                 \
        .clock          = UART3_RX_DMA_CLOCK,                   \
        .trigger_select = UART3_RX_DMA_TRIG_SELECT,             \
        .trigger_event  = EVT_SRC_USART1_RI,                    \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = UART3_RX_DMA_IRQn,                    \
            .irq_prio   = UART3_RX_DMA_INT_PRIO,                \
            .int_src    = UART3_RX_DMA_INT_SRC,                 \
        },                                                      \
    }
#endif /* UART3_DMA_RX_CONFIG */

#ifndef UART3_RXTO_CONFIG
#define UART3_RXTO_CONFIG                                       \
    {                                                           \
        .TMR0_Instance = CM_TMR0_1,                             \
        .channel       = TMR0_CH_A,                             \
        .clock         = FCG2_PERIPH_TMR0_1,                    \
        .timeout_bits  = 20UL,                                  \
        .irq_config    =                                        \
        {                                                       \
            .irq_num   = BSP_UART3_RXTO_IRQ_NUM,                \
            .irq_prio  = BSP_UART3_RXTO_IRQ_PRIO,               \
            .int_src   = INT_SRC_USART1_RTO,                    \
        },                                                      \
    }
#endif /* UART3_RXTO_CONFIG */
#endif /* BSP_UART3_RX_USING_DMA */

#if defined(BSP_UART3_TX_USING_DMA)
#ifndef UART3_DMA_TX_CONFIG
#define UART3_DMA_TX_CONFIG                                     \
    {                                                           \
        .Instance       = UART3_TX_DMA_INSTANCE,                \
        .channel        = UART3_TX_DMA_CHANNEL,                 \
        .clock          = UART3_TX_DMA_CLOCK,                   \
        .trigger_select = UART3_TX_DMA_TRIG_SELECT,             \
        .trigger_event  = EVT_SRC_USART1_TI,                    \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = UART3_TX_DMA_IRQn,                    \
            .irq_prio   = UART3_TX_DMA_INT_PRIO,                \
            .int_src    = UART3_TX_DMA_INT_SRC,                 \
        },                                                      \
    }
#endif /* UART3_DMA_TX_CONFIG */
#endif /* BSP_UART3_TX_USING_DMA */
#endif /* BSP_USING_UART3 */

#ifdef __cplusplus
}
#endif

#endif
