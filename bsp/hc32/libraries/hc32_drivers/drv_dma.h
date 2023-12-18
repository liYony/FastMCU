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

#ifndef __DRV_DMA_H__
#define __DRV_DMA_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <fmcu.h>
#include "drv_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dma_config
{
    CM_DMA_TypeDef          *Instance;
    fm_uint32_t             channel;
    fm_uint32_t             clock;
    fm_uint32_t             trigger_select;
    en_event_src_t          trigger_event;
    struct hc32_irq_config  irq_config;
    func_ptr_t              irq_callback;
};

#ifdef __cplusplus
}
#endif

#endif /* __DRV_DMA_H__ */

