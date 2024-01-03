/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-22     liYony       first version
 */

#include <fmcu.h>
#include <fmdevice.h>

#define DBG_TAG "flash"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

static struct fm_flash_device _hw_flash;

fm_err_t _flash_init(fm_device_t dev)
{
    fm_err_t result = FM_EOK;
    struct fm_flash_device *flash = (struct fm_flash_device *)dev;

    /* check parameters */
    FM_ASSERT(flash != FM_NULL);

    if (flash->ops->init)
    {
        result = flash->ops->init(&flash->start_addr, &flash->end_addr);
    }

    LOG_I("flash init success. flash capacity <%08X - %08X>",
          flash->start_addr, flash->end_addr);

    return result;
}

static fm_ssize_t _flash_read(fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size)
{
    struct fm_flash_device *flash = (struct fm_flash_device *)dev;

    /* check parameters */
    FM_ASSERT(flash != FM_NULL);

    if (pos <= 0 || (flash->start_addr + pos + size >= flash->end_addr) || buffer == FM_NULL)
        return 0;

    if (flash->ops->read)
    {
        return flash->ops->read(flash->start_addr + pos, (fm_uint8_t *)buffer, size);
    }

    return 0;
}

static fm_ssize_t _flash_write(fm_device_t dev, fm_off_t pos, const void *buffer, fm_size_t size)
{
    struct fm_flash_device *flash = (struct fm_flash_device *)dev;

    /* check parameters */
    FM_ASSERT(flash != FM_NULL);

    if (pos <= 0 || (flash->start_addr + pos + size >= flash->end_addr) || buffer == FM_NULL)
        return 0;

    if (flash->ops->write)
    {
        return flash->ops->write(flash->start_addr + pos, (fm_uint8_t *)buffer, size);
    }

    return 0;
}

static fm_err_t _flash_control(fm_device_t dev, int cmd, void *args)
{
    fm_err_t result = FM_EOK;
    struct fm_flash_device *flash = (struct fm_flash_device *)dev;

    /* check parameters */
    FM_ASSERT(flash != FM_NULL);

    if (cmd == FM_FLASH_CMD_ERASE && flash->ops->erase)
    {
        struct fm_flash_erase *erase = (struct fm_flash_erase *)args;
        if (erase->addr <= 0 || erase == FM_NULL ||
            (flash->start_addr + erase->addr + erase->size >= flash->end_addr))
            return -FM_ERROR;

        result = flash->ops->erase(flash->start_addr + erase->addr, erase->size);
    }

    return result;
}

#ifdef FM_USING_DEVICE_OPS
const static struct fm_device_ops flash_ops =
{
    _flash_init,
    FM_NULL,
    FM_NULL,
    _flash_read,
    _flash_write,
    _flash_control,
};
#endif

fm_err_t fm_hw_flash_register(const char *name, const struct fm_flash_ops *ops, void *user_data)
{
    fm_err_t result = FM_EOK;
    FM_ASSERT(ops != FM_NULL);

    _hw_flash.parent.type = FM_Device_Class_MTD;
    _hw_flash.parent.rx_indicate = FM_NULL;
    _hw_flash.parent.tx_complete = FM_NULL;

#ifdef FM_USING_DEVICE_OPS
    _hw_flash.parent.ops = &flash_ops;
#else
    _hw_flash.parent.init = _flash_init;
    _hw_flash.parent.open = FM_NULL;
    _hw_flash.parent.close = FM_NULL;
    _hw_flash.parent.read = _flash_read;
    _hw_flash.parent.write = _flash_write;
    _hw_flash.parent.control = _flash_control;
#endif
    _hw_flash.ops = ops;
    _hw_flash.parent.user_data = (void *)user_data;

    result = fm_device_register(&_hw_flash.parent, name, FM_DEVICE_FLAG_RDWR);

    return result;
}
