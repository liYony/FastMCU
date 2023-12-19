/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-20     Bernard      the first version
 * 2017-10-20     ZYH          add mode open drain and input pull down
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#ifndef __PIN_H__
#define __PIN_H__

#include <fmcu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* pin device and operations for RT-Thread */
struct fm_device_pin
{
    struct fm_device parent;
    const struct fm_pin_ops *ops;
};

#define PIN_NONE                (-1)

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02
#define PIN_MODE_INPUT_PULLDOWN 0x03
#define PIN_MODE_OUTPUT_OD      0x04

#define PIN_IRQ_MODE_RISING             0x00
#define PIN_IRQ_MODE_FALLING            0x01
#define PIN_IRQ_MODE_RISING_FALLING     0x02
#define PIN_IRQ_MODE_HIGH_LEVEL         0x03
#define PIN_IRQ_MODE_LOW_LEVEL          0x04

#define PIN_IRQ_DISABLE                 0x00
#define PIN_IRQ_ENABLE                  0x01

#define PIN_IRQ_PIN_NONE                PIN_NONE

struct fm_device_pin_mode
{
    fm_base_t pin;
    fm_uint8_t mode; /* e.g. PIN_MODE_OUTPUT */
};

struct fm_device_pin_value
{
    fm_base_t pin;
    fm_uint8_t value; /* PIN_LOW or PIN_HIGH */
};

struct fm_pin_irq_hdr
{
    fm_base_t        pin;
    fm_uint8_t       mode; /* e.g. PIN_IRQ_MODE_RISING */
    void (*hdr)(void *args);
    void             *args;
};
struct fm_pin_ops
{
    void (*pin_mode)(struct fm_device *device, fm_base_t pin, fm_uint8_t mode);
    void (*pin_write)(struct fm_device *device, fm_base_t pin, fm_uint8_t value);
    fm_int8_t  (*pin_read)(struct fm_device *device, fm_base_t pin);
    fm_err_t (*pin_attach_irq)(struct fm_device *device, fm_base_t pin,
            fm_uint8_t mode, void (*hdr)(void *args), void *args);
    fm_err_t (*pin_detach_irq)(struct fm_device *device, fm_base_t pin);
    fm_err_t (*pin_irq_enable)(struct fm_device *device, fm_base_t pin, fm_uint8_t enabled);
    fm_base_t (*pin_get)(const char *name);
};

int fm_device_pin_register(const char *name, const struct fm_pin_ops *ops, void *user_data);
void fm_pin_mode(fm_base_t pin, fm_uint8_t mode);
void fm_pin_write(fm_base_t pin, fm_uint8_t value);
fm_int8_t fm_pin_read(fm_base_t pin);
fm_base_t fm_pin_get(const char *name);
fm_err_t fm_pin_attach_irq(fm_base_t pin, fm_uint8_t mode,
                           void (*hdr)(void *args), void  *args);
fm_err_t fm_pin_detach_irq(fm_base_t pin);
fm_err_t fm_pin_irq_enable(fm_base_t pin, fm_uint8_t enabled);

#ifdef __cplusplus
}
#endif

#endif
