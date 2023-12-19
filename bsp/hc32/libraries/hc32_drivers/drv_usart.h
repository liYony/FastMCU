/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#ifndef __DRV_USART_H__
#define __DRV_USART_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <fmcu.h>
#include <fmdevice.h>
#include "drv_irq.h"
#include "drv_dma.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
struct hc32_uart_irq_config
{
    struct hc32_irq_config irq_config;
    func_ptr_t             irq_callback;
};

/* HC32 config Rx timeout */
struct hc32_uart_rxto
{
    CM_TMR0_TypeDef             *TMR0_Instance;
    fm_uint32_t                 channel;
    fm_uint32_t                 clock;
    fm_size_t                   timeout_bits;
    struct hc32_irq_config      irq_config;
    func_ptr_t                  irq_callback;
};

/* HC32 config uart class */
struct hc32_uart_config
{
    const char                  *name;
    CM_USART_TypeDef            *Instance;
    fm_uint32_t                 clock;
    struct hc32_uart_irq_config rx_err_irq;
    struct hc32_uart_irq_config rx_irq;
    struct hc32_uart_irq_config tx_irq;
    struct hc32_uart_irq_config tx_cplt_irq;
#ifdef FM_SERIAL_USING_DMA
    struct hc32_uart_rxto       *rx_timeout;
    stc_dma_llp_descriptor_t    llp_desc;
    struct dma_config           *dma_rx;
    struct dma_config           *dma_tx;
#endif
};

/* HC32 uart dirver class */
struct hc32_uart
{
    struct hc32_uart_config *config;
#ifdef FM_SERIAL_USING_DMA
    fm_size_t               dmarx_remaining_cnt;
#endif
    fm_uint16_t             uart_dma_flag;
    struct fm_serial_device serial;
};

int fm_hw_usart_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_USART_H__ */
