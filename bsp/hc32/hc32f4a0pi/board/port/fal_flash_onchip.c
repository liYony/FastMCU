/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-27     liYony       the first version
 */

#include <fal.h>
#include <fmcu.h>
#include <fmdevice.h>

static fm_flash_device_t flash;

static int init(void)
{
    /* do nothing now */
    flash = (fm_flash_device_t)fm_device_find("flash");
    fm_device_open((fm_device_t)flash, FM_DEVICE_FLAG_RDWR);
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    fm_device_read((fm_device_t)flash, offset, buf, size);
    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    fm_device_write((fm_device_t)flash, offset, buf, size);
    
    return size;
}

static int erase(long offset, size_t size)
{
    struct fm_flash_erase _erase = {offset, size};
    fm_device_control((fm_device_t)flash, FM_FLASH_CMD_ERASE, &_erase);
    
    return size;
}

const struct fal_flash_dev bsp_onchip_flash =
{
    .name       = "flash_onchip",
    .addr       = 0x00000000,
    .len        = EFM_END_ADDR - EFM_START_ADDR,
    .blk_size   = SECTOR_SIZE,
    .ops        = {init, read, write, erase},
    .write_gran = 8
};
