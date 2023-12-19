/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 * 2022-09-24     yuqi         add phase and dead time configuration
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include <fmcu.h>

#define PWM_CMD_ENABLE          (FM_DEVICE_CTRL_BASE(PWM) + 0)
#define PWM_CMD_DISABLE         (FM_DEVICE_CTRL_BASE(PWM) + 1)
#define PWM_CMD_SET             (FM_DEVICE_CTRL_BASE(PWM) + 2)
#define PWM_CMD_GET             (FM_DEVICE_CTRL_BASE(PWM) + 3)
#define PWMN_CMD_ENABLE         (FM_DEVICE_CTRL_BASE(PWM) + 4)
#define PWMN_CMD_DISABLE        (FM_DEVICE_CTRL_BASE(PWM) + 5)
#define PWM_CMD_SET_PERIOD      (FM_DEVICE_CTRL_BASE(PWM) + 6)
#define PWM_CMD_SET_PULSE       (FM_DEVICE_CTRL_BASE(PWM) + 7)
#define PWM_CMD_SET_DEAD_TIME   (FM_DEVICE_CTRL_BASE(PWM) + 8)
#define PWM_CMD_SET_PHASE       (FM_DEVICE_CTRL_BASE(PWM) + 9)
#define PWM_CMD_ENABLE_IRQ      (FM_DEVICE_CTRL_BASE(PWM) + 10)
#define PWM_CMD_DISABLE_IRQ     (FM_DEVICE_CTRL_BASE(PWM) + 11)

struct fm_pwm_configuration
{
    fm_uint32_t channel; /* 0 ~ n or 0 ~ -n, which depends on specific MCU requirements */
    fm_uint32_t period;  /* unit:ns 1ns~4.29s:1Ghz~0.23hz */
    fm_uint32_t pulse;   /* unit:ns (pulse<=period) */
    fm_uint32_t dead_time;  /* unit:ns */
    fm_uint32_t phase;  /*unit: degree, 0~360, which is the phase of pwm output, */
    /*
     * FM_TRUE  : The channel of pwm is complememtary.
     * FM_FALSE : The channel of pwm is nomal.
    */
    fm_bool_t  complementary;
};

struct fm_device_pwm;
struct fm_pwm_ops
{
    fm_err_t (*control)(struct fm_device_pwm *device, int cmd, void *arg);
};

struct fm_device_pwm
{
    struct fm_device parent;
    const struct fm_pwm_ops *ops;
};

fm_err_t fm_device_pwm_register(struct fm_device_pwm *device, const char *name, const struct fm_pwm_ops *ops, const void *user_data);

fm_err_t fm_pwm_enable(struct fm_device_pwm *device, int channel);
fm_err_t fm_pwm_disable(struct fm_device_pwm *device, int channel);
fm_err_t fm_pwm_set(struct fm_device_pwm *device, int channel, fm_uint32_t period, fm_uint32_t pulse);
fm_err_t fm_pwm_set_period(struct fm_device_pwm *device, int channel, fm_uint32_t period);
fm_err_t fm_pwm_set_pulse(struct fm_device_pwm *device, int channel, fm_uint32_t pulse);
fm_err_t fm_pwm_set_dead_time(struct fm_device_pwm *device, int channel, fm_uint32_t dead_time);
fm_err_t fm_pwm_set_phase(struct fm_device_pwm *device, int channel, fm_uint32_t phase);

#endif /* __DRV_PWM_H__ */
