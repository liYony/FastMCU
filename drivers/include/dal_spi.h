#ifndef __DAL_SPI_H__
#define __DAL_SPI_H__

#include <dal_type.h>
#include <dal_gpio.h>

//#define SPI_USING_TRANSFER

#ifndef SPI_USING_TRANSFER
#define CS_SEL_ENABLE   (1)
#define CS_SEL_DISABLE  (0)
#endif

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

#if defined(SPI_USING_TRANSFER)
int mcu_spi_xfer(dal_spi_number_t spi, dal_spi_message_t *msg);
#else
int mcu_spi_writeread(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length);
int mcu_spi_cs_ctl(dal_spi_number_t spi, uint8_t select);
int mcu_spi_wait(dal_spi_number_t spi);
#endif
int mcu_spi_attach_cs(dal_spi_number_t spi, dal_gpio_instance_t cs);
int mcu_spi_init(dal_spi_number_t spi, uint32_t freq);

int dal_spi_attach_cs(dal_spi_number_t spi, dal_gpio_instance_t cs);
int dal_spi_init(dal_spi_number_t spi, uint32_t freq);
#if defined(SPI_USING_TRANSFER)
int dal_spi_transfer(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length);
#else
int dal_spi_writeread(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length);
int dal_spi_cs_ctl(dal_spi_number_t spi, uint8_t select);
int dal_spi_wait(dal_spi_number_t spi);
#endif
#endif // !__DAL_SPI_H__
