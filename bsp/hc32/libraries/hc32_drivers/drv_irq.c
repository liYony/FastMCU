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

#include <fmcu.h>
#include "drv_irq.h"

fm_err_t hc32_install_irq_handler(struct hc32_irq_config *irq_config,
                                  void (*irq_hdr)(void),
                                  fm_bool_t irq_enable)
{
    fm_err_t result = -FM_ERROR;
    stc_irq_signin_config_t stcIrqSignConfig;

    FM_ASSERT(FM_NULL != irq_config);
    FM_ASSERT(FM_NULL != irq_hdr);

    stcIrqSignConfig.enIRQn      = irq_config->irq_num;
    stcIrqSignConfig.enIntSrc    = irq_config->int_src;
    stcIrqSignConfig.pfnCallback = irq_hdr;
    if (LL_OK == INTC_IrqSignIn(&stcIrqSignConfig))
    {
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, irq_config->irq_prio);
        if (FM_TRUE == irq_enable)
        {
            NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
        }
        else
        {
            NVIC_DisableIRQ(stcIrqSignConfig.enIRQn);
        }
        result = FM_EOK;
    }
    return result;
}
