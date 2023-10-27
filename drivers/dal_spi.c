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
