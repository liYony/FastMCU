#include <qk_log.h>
#include <qk_halport.h>
#include <qk_type.h>

#define _LOG_BUF_MIN 32

#if (_LOG_BUF_SIZE < _LOG_BUF_MIN)
#error "error: _LOG_BUF_SIZE < 32"
#endif

static uint8_t       qk_log_buff[_LOG_BUF_SIZE];

#if defined(LOG_WITH_PARAM)
static const uint8_t qk_log_param_table[4] = {_LOG_I_PARAM, _LOG_W_PARAM, _LOG_E_PARAM, _LOG_D_PARAM};
static const char    qk_log_prefix[4]     = {'I', 'W', 'E', 'D'};
#endif

#if defined(__ARMCC_VERSION)
#define QK_FPUTC int fputc(int c, FILE *f)
#elif defined(__GNUC__)
#define QK_FPUTC int __io_putchar(int c)
#else
#define QK_FPUTC int fputc(int c, FILE *f)
#endif

QK_FPUTC
{
    uint8_t ch = c & 0xff;

    qk_hal_log_write(&ch, 1);

    return c;
}

void qk_log_output(void *p)
{
    qk_hal_log_write(p, strlen(p));
}

void qk_log_out(uint8_t type, const char *ptr_file, const char *ptr_func, uint32_t line,
             const char *fmt, ...)
{
    uint32_t buf_len  = 0;
    int      str_len  = 0;
    char    *pbuf = (char *)qk_log_buff;
#if defined(LOG_WITH_PARAM)
    uint32_t param    = 0;
    uint8_t  name_len = 0;
    char    *p_tmp = NULL;
    char     line_number[8];
#endif
    
    va_list  ap;

    if (type >= 4)
    {
        return;
    }

    memset(pbuf, 0, _LOG_BUF_SIZE);
#if defined(LOG_WITH_PARAM)
    param           = qk_log_param_table[type];
    
    pbuf[buf_len++] = '[';
    pbuf[buf_len++] = qk_log_prefix[type];
    pbuf[buf_len++] = ':';
    if (param & _LOG_PARAM_FILE)
    {
        pbuf[buf_len++] = '/';
        name_len = strlen(ptr_file);
        p_tmp    = (char *)ptr_file;
        if (name_len > 10)
        {
            p_tmp    = (char *)(ptr_file + name_len - 10);
            name_len = 10;
        }
        memcpy(pbuf + buf_len, p_tmp, name_len);
        buf_len += name_len;
    }
    if (param & _LOG_PARAM_FUNC)
    {
        pbuf[buf_len++] = '/';
        p_tmp    = (char *)ptr_func;
        name_len = strlen(ptr_func);
        memcpy(pbuf + buf_len, p_tmp, name_len);
        buf_len += name_len;
    }
    if (param & _LOG_PARAM_LINE)
    {
        pbuf[buf_len++] = '/';
        str_len = sprintf(line_number, "%d", line);
        if (str_len < 0)
        {
            return;
        }
        memcpy(pbuf + buf_len, line_number, str_len);
        buf_len += str_len;
    }
    pbuf[buf_len++] = ']';
    pbuf[buf_len++] = ' ';
#endif /* LOG_WITH_PARAM */

    va_start(ap, fmt);
    str_len = vsnprintf(pbuf + buf_len, _LOG_BUF_SIZE - buf_len - 1, fmt, ap);
    va_end(ap);
    if (str_len < 0)
    {
        return;
    }
    qk_log_output(pbuf);
}

