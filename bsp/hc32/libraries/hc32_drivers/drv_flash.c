/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-22     liYony       first version
 */

#include <fmdevice.h>
#include "hc32_ll.h"

#define DBG_TAG "drv.flash"
#define DBG_LVL DBG_LOG
#include <fmdbg.h>

#ifdef BSP_USING_FLASH

static int32_t FLASH_ReadData(uint32_t u32Addr, uint8_t *pu8Buff, uint32_t u32Len)
{
    uint32_t i;
    uint32_t u32WordLength, u8ByteRemain;
    uint32_t *pu32ReadBuff;
    __IO uint32_t *pu32FlashAddr;
    uint8_t *pu8Byte;
    __IO uint8_t *pu8FlashAddr;

    if (0UL != (u32Addr % 4U))
    {
        return LL_ERR_ADDR_ALIGN;
    }

    pu32ReadBuff = (uint32_t *)(uint32_t)pu8Buff;
    pu32FlashAddr = (uint32_t *)u32Addr;
    u32WordLength = u32Len / 4U;
    u8ByteRemain = u32Len % 4U;
    /* Read data */
    for (i = 0UL; i < u32WordLength; i++)
    {
        *(pu32ReadBuff++) = *(pu32FlashAddr++);
    }
    if (0UL != u8ByteRemain)
    {
        pu8Byte = (uint8_t *)pu32ReadBuff;
        pu8FlashAddr = (uint8_t *)pu32FlashAddr;
        for (i = 0UL; i < u8ByteRemain; i++)
        {
            *(pu8Byte++) = *(pu8FlashAddr++);
        }
    }

    return LL_OK;
}

static int32_t FLASH_WriteData(uint32_t u32Addr, uint8_t *pu8Buff, uint32_t u32Len)
{
    if (0UL != (u32Addr % 4U))
    {
        return LL_ERR_ADDR_ALIGN;
    }

    return EFM_Program(u32Addr, pu8Buff, u32Len);
}

static int32_t FLASH_EraseSector(uint32_t u32Addr, uint32_t u32Size)
{
    uint32_t i;
    uint32_t u32PageNum;

    if (u32Size == 0U)
    {
        return EFM_SectorErase(u32Addr);
    }
    else
    {
        u32PageNum = u32Size / SECTOR_SIZE;
        if ((u32Size % SECTOR_SIZE) != 0UL)
        {
            u32PageNum += 1U;
        }
        for (i = 0; i < u32PageNum; i++)
        {
            if (LL_OK != EFM_SectorErase(u32Addr + (i * SECTOR_SIZE)))
            {
                return LL_ERR;
            }
        }
    }

    return LL_OK;
}

static int32_t FLASH_EnableSector(uint32_t u32Addr, uint32_t u32Size, en_functional_state_t enNewState)
{
    uint32_t s_sector, e_sector;

    s_sector = u32Addr / SECTOR_SIZE;
    e_sector = (u32Addr + u32Size) / SECTOR_SIZE;

    EFM_REG_Unlock();
    EFM_FWMC_Cmd(ENABLE);

    if (s_sector == e_sector)
    {
        (void)EFM_SingleSectorOperateCmd(s_sector, enNewState);
    }
    else
    {
        (void)EFM_SequenceSectorOperateCmd(s_sector, (e_sector - s_sector) + 1, enNewState);
    }

    return LL_OK;
}

static fm_err_t hc32_flash_init(fm_uint32_t *saddr, fm_uint32_t *eaddr)
{
    /* Do some flash initialization */

    /* Set flash start and end addr */
    *saddr = EFM_START_ADDR;
    *eaddr = EFM_END_ADDR;

    return FM_EOK;
}

static fm_size_t hc32_flash_read(fm_uint32_t addr, fm_uint8_t *buf, fm_size_t size)
{
    LOG_D("read %d bytes : %08X to %08X", size, addr, addr + size);

    if (FLASH_ReadData(addr, buf, size) != LL_OK)
        return 0;

    return size;
}

static fm_size_t hc32_flash_write(fm_uint32_t addr, const fm_uint8_t *buf, fm_size_t size)
{
    LOG_D("write %d bytes : %08X to %08X", size, addr, addr + size);

    FLASH_EnableSector(addr, size, ENABLE);
    FLASH_EraseSector(addr, size);
    if (FLASH_WriteData(addr, (fm_uint8_t *)buf, size) != LL_OK)
        return 0;
    FLASH_EnableSector(addr, size, DISABLE);

    return size;
}

static fm_err_t hc32_flash_erase(fm_uint32_t addr, fm_size_t size)
{
    LOG_D("erase %d bytes : %08X to %08X", size, addr, addr + size);

    FLASH_EnableSector(addr, size, ENABLE);
    if (FLASH_EraseSector(addr, size) != LL_OK)
        return -FM_ERROR;
    FLASH_EnableSector(addr, size, DISABLE);

    return FM_EOK;
}

struct fm_flash_ops hc32_flash_ops =
{
    .init = hc32_flash_init,
    .read = hc32_flash_read,
    .write = hc32_flash_write,
    .erase = hc32_flash_erase
};

int fm_hw_flash_init(void)
{
    fm_hw_flash_register("flash", &hc32_flash_ops, FM_NULL);
    return 0;
}

INIT_BOARD_EXPORT(fm_hw_flash_init);

#endif /* BSP_USING_FLASH */
