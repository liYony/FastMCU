/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-22     liYony       first version
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#include <fmcu.h>

struct fm_flash_device;
struct fm_flash_ops
{
    fm_err_t (*init)(fm_uint32_t *saddr, fm_uint32_t *eaddr);
    fm_size_t (*read)(fm_uint32_t addr, fm_uint8_t *buf, fm_size_t size);
    fm_size_t (*write)(fm_uint32_t addr, const fm_uint8_t *buf, fm_size_t size);
    fm_err_t (*erase)(fm_uint32_t addr, fm_size_t size);
};

struct fm_flash_erase
{
    fm_uint32_t addr;
    fm_size_t size;
};
typedef struct fm_flash_erase *fm_flash_erase_t;

struct fm_flash_device
{
    struct fm_device parent;
    fm_uint32_t start_addr;
    fm_uint32_t end_addr;

    const struct fm_flash_ops *ops;
};
typedef struct fm_flash_device *fm_flash_device_t;

typedef enum
{
    FM_FLASH_CMD_ERASE = FM_DEVICE_CTRL_BASE(MTD) + 1,
} fm_flash_cmd_t;

fm_err_t fm_hw_flash_register(const char *name, const struct fm_flash_ops *ops, void *user_data);

#endif /* __FLASH_H__ */
