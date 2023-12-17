#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <fmcu.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BAUD_RATE_2400                  2400
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_19200                 19200
#define BAUD_RATE_38400                 38400
#define BAUD_RATE_57600                 57600
#define BAUD_RATE_115200                115200
#define BAUD_RATE_230400                230400
#define BAUD_RATE_460800                460800
#define BAUD_RATE_500000                500000
#define BAUD_RATE_921600                921600
#define BAUD_RATE_2000000               2000000
#define BAUD_RATE_2500000               2500000
#define BAUD_RATE_3000000               3000000

#define DATA_BITS_5                     5
#define DATA_BITS_6                     6
#define DATA_BITS_7                     7
#define DATA_BITS_8                     8
#define DATA_BITS_9                     9

#define STOP_BITS_1                     0
#define STOP_BITS_2                     1
#define STOP_BITS_3                     2
#define STOP_BITS_4                     3

#define PARITY_NONE                     0
#define PARITY_ODD                      1
#define PARITY_EVEN                     2

#define BIT_ORDER_LSB                   0
#define BIT_ORDER_MSB                   1

#define NRZ_NORMAL                      0       /* Non Return to Zero : normal mode */
#define NRZ_INVERTED                    1       /* Non Return to Zero : inverted mode */

#define FM_DEVICE_FLAG_RX_BLOCKING      0x1000
#define FM_DEVICE_FLAG_RX_NON_BLOCKING  0x2000

#define FM_DEVICE_FLAG_TX_BLOCKING      0x4000
#define FM_DEVICE_FLAG_TX_NON_BLOCKING  0x8000

#define FM_SERIAL_RX_BLOCKING           FM_DEVICE_FLAG_RX_BLOCKING
#define FM_SERIAL_RX_NON_BLOCKING       FM_DEVICE_FLAG_RX_NON_BLOCKING
#define FM_SERIAL_TX_BLOCKING           FM_DEVICE_FLAG_TX_BLOCKING
#define FM_SERIAL_TX_NON_BLOCKING       FM_DEVICE_FLAG_TX_NON_BLOCKING

#define FM_DEVICE_CHECK_OPTMODE         0x20

#define FM_SERIAL_EVENT_RX_IND          0x01    /* Rx indication */
#define FM_SERIAL_EVENT_TX_DONE         0x02    /* Tx complete   */
#define FM_SERIAL_EVENT_RX_DMADONE      0x03    /* Rx DMA transfer done */
#define FM_SERIAL_EVENT_TX_DMADONE      0x04    /* Tx DMA transfer done */
#define FM_SERIAL_EVENT_RX_TIMEOUT      0x05    /* Rx timeout    */

#define FM_SERIAL_ERR_OVERRUN           0x01
#define FM_SERIAL_ERR_FRAMING           0x02
#define FM_SERIAL_ERR_PARITY            0x03

#define FM_SERIAL_TX_DATAQUEUE_SIZE     2048
#define FM_SERIAL_TX_DATAQUEUE_LWM      30

#define FM_SERIAL_RX_MINBUFSZ 64
#define FM_SERIAL_TX_MINBUFSZ 64

#define FM_SERIAL_TX_BLOCKING_BUFFER    1
#define FM_SERIAL_TX_BLOCKING_NO_BUFFER 0

#define FM_SERIAL_FLOWCONTROL_CTSRTS    1
#define FM_SERIAL_FLOWCONTROL_NONE      0

/* Default config for serial_configure structure */
#define FM_SERIAL_CONFIG_DEFAULT                      \
{                                                     \
    BAUD_RATE_115200,           /* 115200 bits/s */   \
    DATA_BITS_8,                /* 8 databits */      \
    STOP_BITS_1,                /* 1 stopbit */       \
    PARITY_NONE,                /* No parity  */      \
    BIT_ORDER_LSB,              /* LSB first sent */  \
    NRZ_NORMAL,                 /* Normal mode */     \
    FM_SERIAL_RX_MINBUFSZ,      /* rxBuf size */      \
    FM_SERIAL_TX_MINBUFSZ,      /* txBuf size */      \
    FM_SERIAL_FLOWCONTROL_NONE, /* Off flowcontrol */ \
    0                                                 \
}

struct serial_configure
{
    fm_uint32_t baud_rate;

    fm_uint32_t data_bits               :4;
    fm_uint32_t stop_bits               :2;
    fm_uint32_t parity                  :2;
    fm_uint32_t bit_order               :1;
    fm_uint32_t invert                  :1;
    fm_uint32_t rx_bufsz                :16;
    fm_uint32_t tx_bufsz                :16;
    fm_uint32_t flowcontrol             :1;
    fm_uint32_t reserved                :5;
};

/*
 * Serial Receive FIFO mode
 */
struct fm_serial_rx_fifo
{
    struct fm_ringbuffer rb;

    struct fm_completion rx_cpt;

    fm_uint16_t rx_cpt_index;

    /* software fifo */
    fm_uint8_t buffer[];
};

/*
 * Serial Transmit FIFO mode
 */
struct fm_serial_tx_fifo
{
    struct fm_ringbuffer rb;

    fm_size_t put_size;

    fm_bool_t activated;

    struct fm_completion tx_cpt;

    /* software fifo */
    fm_uint8_t buffer[];
};

struct fm_serial_device
{
    struct fm_device            parent;

    const struct fm_serial_ops *ops;
    struct serial_configure     config;

    void *serial_rx;
    void *serial_tx;
};

/**
 * serial operators
 */
struct fm_serial_ops
{
    fm_err_t (*configure)(struct fm_serial_device       *serial,
                          struct serial_configure       *cfg);

    fm_err_t (*control)(struct fm_serial_device         *serial,
                                            int          cmd,
                                            void        *arg);

    int (*putc)(struct fm_serial_device *serial, char c);
    int (*getc)(struct fm_serial_device *serial);

    fm_ssize_t (*transmit)(struct fm_serial_device       *serial,
                                 fm_uint8_t             *buf,
                                 fm_size_t               size,
                                 fm_uint32_t             tx_flag);
};

void fm_hw_serial_isr(struct fm_serial_device *serial, int event);

fm_err_t fm_hw_serial_register(struct fm_serial_device      *serial,
                               const  char                  *name,
                                      fm_uint32_t            flag,
                                      void                  *data);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_H__ */
