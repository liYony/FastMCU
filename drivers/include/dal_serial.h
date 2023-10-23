#ifndef __DAL_SERIAL_H__
#define __DAL_SERIAL_H__

#include <dal.h>
#include <dal_it.h>

typedef enum
{
    DAL_HAL_UART_1,
    DAL_HAL_UART_2,
    DAL_HAL_UART_3,
    DAL_HAL_UART_4,
    DAL_HAL_UART_5,
    DAL_HAL_UART_6,
    DAL_HAL_UART_7,
    DAL_HAL_UART_8,
    DAL_HAL_LPUART_1,
    DAL_HAL_UART_NUMBER
} dal_uart_number_t;

typedef int (*dal_uart_idle_cb_t)(uint8_t *pbuf, uint16_t len, void *user_data);

typedef struct
{
    dal_uart_number_t       uart;
    dal_it_t               it;
    uint8_t               *pbuf;
    uint16_t               len;
    uint16_t               idle_ms;
    dal_uart_idle_cb_t callback;
    void                  *user_data;
    volatile uint16_t      index;
    volatile uint16_t      l_index;
    volatile uint32_t      l_tick;
    struct list_head       list;
} dal_uart_idle_attr_t;

/**
 * \}
 */

/**
 * \defgroup UART_Exported_Definitions
 * \{
 */
/// 创建 attr
#define DAL_UART_CREATE_ATTR(name, buf_len, ms, cb, ud) \
    static uint8_t            ibuf##name[buf_len];       \
    static dal_uart_idle_attr_t name = {                   \
        .pbuf      = ibuf##name,                         \
        .len       = buf_len,                            \
        .idle_ms   = ms,                                 \
        .callback  = cb,                                 \
        .user_data = ud,                                 \
        .index     = 0,                                  \
        .l_index   = 0,                                  \
        .l_tick    = 0,                                  \
    }

/// 对已有的attr进行初始化
#define DAL_UART_INIT_ATTR(pattr, buf, buf_len, ms, cb, ud) \
    do                                                       \
    {                                                        \
        (pattr)->pbuf      = buf;                            \
        (pattr)->len       = buf_len;                        \
        (pattr)->idle_ms   = ms;                             \
        (pattr)->callback  = cb;                             \
        (pattr)->user_data = ud;                             \
        (pattr)->index     = 0;                              \
        (pattr)->l_index   = 0;                              \
        (pattr)->l_tick    = 0;                              \
    } while (0)

/**
 * \}
 */

/**
 * \defgroup UART_Exported_Functions
 * \{
 */

int mcu_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len);
int mcu_uart_receive(dal_uart_number_t uart, uint8_t *pbuf, uint16_t len);
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int dal_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len);
int dal_uart_receive(dal_uart_number_t uart, uint8_t *pbuf, uint16_t len);

/// attr 通过 bHAL_UART_CREATE_ATTR 创建后传入。
/// 或者已有attr变量，通过 bHAL_UART_INIT_ATTR 初始化后传入。
int dal_uart_receive_idle(dal_uart_number_t uart, dal_uart_idle_attr_t *attr);

#endif
