#ifndef __DAL_FLASH_H__
#define __DAL_FLASH_H__

#include <dal_type.h>

int      mcu_flash_erase(uint32_t addr, uint32_t size);
int      mcu_flash_write(uint32_t addr, const uint8_t *pbuf, uint32_t size);
int      mcu_flash_read(uint32_t addr, uint8_t *pbuf, uint32_t size);
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int      dal_flash_erase(uint32_t addr, uint32_t size);
int      dal_flash_write(uint32_t addr, const uint8_t *pbuf, uint32_t size);
int      dal_flash_read(uint32_t addr, uint8_t *pbuf, uint32_t size);

#endif /* __DAL_FLASH_H__ */
