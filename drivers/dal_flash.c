#include <dal_flash.h>

dal_weak int mcu_flash_erase(uint32_t addr, uint32_t size)
{
    return -1;
}

dal_weak int mcu_flash_write(uint32_t addr, const uint8_t *pbuf, uint32_t size)
{
    return -1;
}

dal_weak int mcu_flash_read(uint32_t addr, uint8_t *pbuf, uint32_t size)
{
    return -1;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

int dal_flash_erase(uint32_t addr, uint32_t size)
{
    return mcu_flash_erase(addr, size);
}

int dal_flash_write(uint32_t addr, const uint8_t *pbuf, uint32_t size)
{
    return mcu_flash_write(addr, pbuf, size);
}

int dal_flash_read(uint32_t addr, uint8_t *pbuf, uint32_t size)
{
    return mcu_flash_read(addr, pbuf, size);
}
