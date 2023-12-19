/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-10     SummerGift   first version
 * 2020-10-14     Dozingfiretruck   Porting for stm32wbxx
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#ifndef __DRV_DMA_H_
#define __DRV_DMA_H_

#include <fmcu.h>
#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32L5)\
    || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32G0) \
    || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB)|| defined(SOC_SERIES_STM32F3) \
    || defined(SOC_SERIES_STM32U5) || defined(SOC_SERIES_STM32H5)
#define DMA_INSTANCE_TYPE              DMA_Channel_TypeDef
#elif defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7)\
    || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1)
#define DMA_INSTANCE_TYPE              DMA_Stream_TypeDef
#endif /*  defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL)  */

struct dma_config {
    DMA_INSTANCE_TYPE *Instance;
    fm_uint32_t dma_rcc;
    IRQn_Type dma_irq;

#if defined(SOC_SERIES_STM32F2) || defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7)|| defined(SOC_SERIES_STM32F3)
    fm_uint32_t channel;
#endif

#if defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL)  || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4)\
    || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32WB) || defined(SOC_SERIES_STM32L5)
    fm_uint32_t request;
#endif
};

#ifdef __cplusplus
}
#endif

#endif /*__DRV_DMA_H_ */
