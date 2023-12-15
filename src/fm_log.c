#include <fm_log.h>

#include <stdio.h>
#include <stdarg.h>
#include "hc32_ll.h"
extern int mcu_uart_send(const uint8_t *pbuf, uint16_t len);
#include <string.h>
#if defined(DEBUG_ENABLE)

#define _DBG_BUF_MIN 32

#if (_DBG_BUF_SIZE < _DBG_BUF_MIN)
#error "error: _LOG_BUF_SIZE < 32"
#endif

static uint8_t       fm_log_buff[_DBG_BUF_SIZE];

#if defined(__ARMCC_VERSION)
#define FM_FPUTC int fputc(int c, FILE *f)
#elif defined(__GNUC__)
#define FM_FPUTC int __io_putchar(int c)
#else
#define FM_FPUTC int fputc(int c, FILE *f)
#endif

FM_FPUTC
{
    uint8_t ch = c & 0xff;

    mcu_uart_send(&ch, 1);

    return c;
}

void fm_log_output(void *p)
{
    mcu_uart_send(p, strlen(p));
}

int fm_kprintf(const char *fmt, ...)
{
    va_list args;
    uint16_t length = 0;
    
    char    *pbuf = (char *)fm_log_buff;

    va_start(args, fmt);

    length = vsnprintf(pbuf, _DBG_BUF_SIZE - 1, fmt, args);
    if (length > _DBG_BUF_SIZE - 1)
    {
        length = _DBG_BUF_SIZE - 1;
    }

    fm_log_output(pbuf);

    va_end(args);

    return length;
}

int fm_putchar(uint8_t c)
{
    mcu_uart_send(&c, 1);
    return c;
}

#endif /* DEBUG_ENABLE */
