/*
 * Copyright (c) 2006-2022, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2023-12-21     liYony            first version
 */

#ifndef __DRV_TMRA_H__
#define __DRV_TMRA_H__

#include <fmcu.h>
#include <fmdevice.h>
#include <board.h>
#include "drv_irq.h"

struct hc32_tim_tmra
{
    struct fm_hwtimer_device    device;
    CM_TMRA_TypeDef             *instance;
    func_ptr_t                  irq_callback;
    struct hc32_irq_config      irq_config;
    fm_uint32_t                 clk_freq;
    char                        *name;
};

#endif /* __DRV_TMRA_H__ */
