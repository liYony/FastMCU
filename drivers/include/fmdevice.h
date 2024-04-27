/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-19     liYony       first version
 * 2023-12-22     liYony       add flash driver
 */

#ifndef __FM_DEVICE_H__
#define __FM_DEVICE_H__

#include <fmcu.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FM_DEVICE(device)            ((fm_device_t)device)

#ifdef FM_USING_SERIAL
#include "serial.h"
#endif /* FM_USING_SERIAL */

#ifdef FM_USING_PIN
#include "pin.h"
#endif /* FM_USING_SERIAL */

#ifdef FM_USING_ADC
#include "adc.h"
#endif /* FM_USING_SERIAL */

#ifdef FM_USING_PWM
#include "pwm.h"
#endif /* FM_USING_PWM */

#ifdef FM_USING_I2C
#include "i2c.h"
#include "i2c_dev.h"

#ifdef FM_USING_I2C_BITOPS
#include "i2c-bit-ops.h"
#endif /* FM_USING_I2C_BITOPS */
#endif /* FM_USING_I2C */

#ifdef FM_USING_HWTIMER
#include "hwtimer.h"
#endif /* FM_USING_HWTIMER */

#ifdef FM_USING_FLASH
#include "flash.h"
#endif /* FM_USING_FLASH */

#ifdef FM_USING_DEV_BUS
#include "bus.h"
#include "driver.h"
#ifdef FM_USING_DEV_BUS_PLATFORM
#include "platform.h"
#endif /* FM_USING_DEV_BUS_PLATFORM */
#endif /* FM_USING_DEV_BUS */

#ifdef __cplusplus
}
#endif

#endif /* __FM_DEVICE_H__ */
