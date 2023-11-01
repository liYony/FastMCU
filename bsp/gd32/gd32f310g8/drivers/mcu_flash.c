#include <dal.h>
#include "gd32f3x0.h"

#define MCU_FLASH_START (0x08000000UL) /*!< FLASH(up to 64kb) base address  */
#define MCU_FLASH_END (0x0800FFFFUL)   /*!< FLASH END address               */
#define MCU_FLASH_PAGE_SIZE (0x400UL)

static uint8_t mcu_flash_page(uint32_t addr)
{
    return (addr - MCU_FLASH_START) / MCU_FLASH_PAGE_SIZE;
}

int mcu_flash_erase(uint32_t addr, uint32_t size)
{
    uint8_t first_page = 0, pages_num = 0, erase_page = 0;
    fmc_state_enum erase_result = FMC_READY;

    if (((addr + size) > MCU_FLASH_END) &&
        ((addr - MCU_FLASH_START) % MCU_FLASH_PAGE_SIZE != 0))
    {
        return -1;
    }

    first_page = mcu_flash_page(addr);
    pages_num = mcu_flash_page(addr + size - 1) - first_page + 1;

    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    for (erase_page = 0; (erase_page < pages_num) && (erase_result == FMC_READY); erase_page++)
    {
        erase_result = fmc_page_erase(MCU_FLASH_START + (MCU_FLASH_PAGE_SIZE * (first_page + erase_page)));

        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    fmc_lock();

    if (erase_result != FMC_READY)
    {
        return -1;
    }

    return 0;
}

int mcu_flash_write(uint32_t addr, const uint8_t *pbuf, uint32_t size)
{
    uint8_t first_page = 0, pages_num = 0, erase_page = 0;
    uint32_t write_off = 0;
    fmc_state_enum erase_result = FMC_READY;

    if (((addr + size) > MCU_FLASH_END) &&
        ((addr - MCU_FLASH_START) % MCU_FLASH_PAGE_SIZE != 0))
    {
        return -1;
    }

    first_page = mcu_flash_page(addr);
    pages_num = mcu_flash_page(addr + size - 1) - first_page + 1;

    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    for (erase_page = 0; (erase_page < pages_num) && (erase_result == FMC_READY); erase_page++)
    {
        uint32_t page_a_start = MCU_FLASH_START + (MCU_FLASH_PAGE_SIZE * (first_page + erase_page));

        erase_result = fmc_page_erase(page_a_start);
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

        uint16_t page_w_size = (size - MCU_FLASH_PAGE_SIZE * erase_page) / MCU_FLASH_PAGE_SIZE > 0 ? \
                                MCU_FLASH_PAGE_SIZE : (size - MCU_FLASH_PAGE_SIZE * erase_page);

        log_d("page addr <0x%08X> write %d bytes.\r\n", page_a_start, page_w_size);

        for (write_off = 0; (write_off < page_w_size) && (erase_result == FMC_READY); write_off += 4)
        {
            erase_result = fmc_word_program(page_a_start + write_off, *(uint32_t *)(pbuf + write_off));
            fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
        }

        uint16_t verify_size = 0;

        for (verify_size = 0; verify_size < page_w_size; verify_size++, pbuf++, addr++)
        {
            if (*pbuf != *(uint8_t *)addr)
            {
                erase_result = FMC_PGERR;
                log_d("verification failed.\r\n");
                break;
            }
        }
    }

    fmc_lock();

    if (erase_result != FMC_READY)
    {
        return -1;
    }

    log_d("flash write success.\r\n");

    return 0;
}

int mcu_flash_read(uint32_t addr, uint8_t *pbuf, uint32_t size)
{
    uint32_t i;

    if ((addr + size) > MCU_FLASH_END)
    {
        return -1;
    }

    for (i = 0; i < size; i++, pbuf++, addr++)
    {
        *pbuf = *(uint8_t *)addr;
    }

    return size;
}

//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------------//
#include <fm_section.h>

#define TEST_FLASH_BASE 0x0800C000UL
#define SIZE 2
uint8_t write_buffer[SIZE] = {0};
void flash_test(void)
{
    dal_flash_erase(TEST_FLASH_BASE + 0x200, 0x300);
    int i = 0;
    for (i = 0; i < SIZE; i++)
    {
        write_buffer[i] = i % 256;
    }
    write_buffer[SIZE - 1] = 0x55;
    dal_flash_write(TEST_FLASH_BASE, write_buffer, SIZE);
    //    dal_flash_write(TEST_FLASH_BASE + 0x200, write_buffer, 100);
    uint8_t read_buffer[100] = {0};
    dal_flash_read(MCU_FLASH_START, read_buffer, 100);
}

INITLV4_EXPORT(flash_test);
