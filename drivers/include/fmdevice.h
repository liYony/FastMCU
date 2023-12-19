/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-19     liYony       first version
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

#ifdef __cplusplus
}
#endif

#endif /* __FM_DEVICE_H__ */
