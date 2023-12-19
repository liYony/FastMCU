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

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include <fmdevice.h>
#include "drv_irq.h"

#if defined(FM_USING_PIN)

#define __HC_PORT(port)                 GPIO_PORT_##port
#define GET_PIN(PORT, PIN)              (fm_base_t)(((fm_uint16_t)__HC_PORT(PORT) * 16) + PIN)

struct hc32_pin_irq_map
{
    fm_uint16_t             pinbit;
    func_ptr_t              irq_callback;
    struct hc32_irq_config  irq_config;
};

int fm_hw_pin_init(void);

#endif

#endif /* __DRV_GPIO_H__ */
