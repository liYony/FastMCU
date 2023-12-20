/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2006-2022, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-06-27     lianghongquan     first version
 * 2023-12-20     liYony            Adaptation HC32F4A0 chip
 */

#ifndef __DRV_PWM_TMRA_H__
#define __DRV_PWM_TMRA_H__

#include <fmcu.h>
#include <fmdevice.h>
#include <board.h>

struct hc32_pwm_tmra
{
    struct fm_device_pwm pwm_device;
    CM_TMRA_TypeDef *instance;
    stc_tmra_init_t stcTmraInit;
    stc_tmra_pwm_init_t stcPwmInit;
    fm_uint8_t channel;
    char *name;
};

#endif /* __DRV_PWM_TMRA_H__ */
