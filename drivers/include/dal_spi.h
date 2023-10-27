#ifndef __DAL_SPI_H__
#define __DAL_SPI_H__

#include <dal_type.h>
#include <dal_gpio.h>

typedef enum
{
    DAL_SPI_1,
    DAL_SPI_2,
    DAL_SPI_3,
    DAL_SPI_4,
    DAL_SPI_INVALID,
} dal_spi_number_t;

typedef struct 
{
    const void *send_buf;
    void *recv_buf;
    uint32_t length;
    unsigned cs_take    : 1;
    unsigned cs_release : 1;
} dal_spi_message_t;

int mcu_spi_xfer(dal_spi_number_t spi, dal_spi_message_t *msg);
int mcu_spi_init(dal_spi_number_t spi, uint32_t freq);

int dal_spi_init(dal_spi_number_t spi, uint32_t freq);
int dal_spi_transfer(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length);

#endif // !__DAL_SPI_H__
