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

#ifndef __DRV_IRQ_H__
#define __DRV_IRQ_H__

#include <fmdef.h>
#include "hc32_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct hc32_irq_config
{
    IRQn_Type       irq_num;
    uint32_t        irq_prio;
    en_int_src_t    int_src;
};

fm_err_t hc32_install_irq_handler(struct hc32_irq_config *irq_config,
                                  void (*irq_hdr)(void),
                                  fm_bool_t irq_enable);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_IRQ_H__ */
