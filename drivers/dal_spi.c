#include <dal_spi.h>

dal_weak int mcu_spi_xfer(dal_spi_number_t spi, dal_spi_message_t *msg)
{
    return -1;
}

dal_weak int mcu_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    return -1;
}

int dal_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    return mcu_spi_init(spi, freq);
}

