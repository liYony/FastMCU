#include <dal_spi.h>

#if defined(SPI_USING_TRANSFER)
dal_weak int mcu_spi_xfer(dal_spi_number_t spi, dal_spi_message_t *msg)
{
    return -1;
}
#else
dal_weak int mcu_spi_writeread(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length)
{
    return -1;
}

dal_weak int mcu_spi_cs_ctl(dal_spi_number_t spi, uint8_t select)
{
    return -1;
}

dal_weak int mcu_spi_wait(dal_spi_number_t spi)
{
    return -1;
}
#endif
dal_weak int mcu_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    return -1;
}

int dal_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    return mcu_spi_init(spi, freq);
}

#if defined(SPI_USING_TRANSFER)
int dal_spi_transfer(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length)
{
    dal_spi_message_t msg;
    
    msg.send_buf   = send_buf;
    msg.recv_buf   = recv_buf;
    msg.length     = length;
    msg.cs_take    = 1;
    msg.cs_release = 1;
    
    return mcu_spi_xfer(spi, &msg);
}
#else
int dal_spi_writeread(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length)
{
    return mcu_spi_writeread(spi, send_buf, recv_buf, length);
}

int dal_spi_cs_ctl(dal_spi_number_t spi, uint8_t select)
{
    return mcu_spi_cs_ctl(spi, select);
}

int dal_spi_wait(dal_spi_number_t spi)
{
    return mcu_spi_wait(spi);
}
#endif
