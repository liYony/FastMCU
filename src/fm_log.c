#include <fm_log.h>
#include <dal.h>

#if defined(DBG_ENABLE)

#define _DBG_BUF_MIN 32

#if (_DBG_BUF_SIZE < _DEBUG_BUF_MIN)
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

    dal_log_output(&ch, 1);

    return c;
}

void fm_log_output(void *p)
{
    dal_log_output(p, strlen(p));
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
    dal_log_output(&c, 1);
    return c;
}

int fm_getchar(uint8_t *c)
{
    return dal_log_input(c, 1);
}

#endif /* DBG_ENABLE */
