#ifndef __FM_LOG_H__
#define __FM_LOG_H__

#include <stdint.h>
#include <stdio.h>
#include <fmconfig.h>

#if defined(DBG_ENABLE)

/* DEBUG buffer size */
#ifndef DBG_BUF_SIZE
#define DBG_BUF_SIZE (512)
#endif

#define _DBG_BUF_SIZE DBG_BUF_SIZE

/* DEBUG level */
#define DBG_ERROR           0
#define DBG_WARNING         1
#define DBG_INFO            2
#define DBG_LOG             3

/* DEBUG tag */
#ifdef DBG_TAG
#ifndef DBG_SECTION_NAME
#define DBG_SECTION_NAME    DBG_TAG
#endif
#else
#ifndef DBG_SECTION_NAME
#define DBG_SECTION_NAME    "DBG"
#endif
#endif /* DBG_TAG */

#ifdef DBG_LVL
#ifndef DBG_LEVEL
#define DBG_LEVEL         DBG_LVL
#endif
#else
#ifndef DBG_LEVEL
#define DBG_LEVEL         DBG_WARNING
#endif
#endif /* DBG_LVL */

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#ifdef DBG_COLOR
#define _DBG_COLOR(n)        fm_kprintf("\033["#n"m")
#define _DBG_LOG_HDR(lvl_name, color_n)                    \
    fm_kprintf("\033["#color_n"m[" lvl_name "/" DBG_SECTION_NAME "] ")
#define _DBG_LOG_X_END                                     \
    fm_kprintf("\033[0m\r\n")
#else
#define _DBG_COLOR(n)
#define _DBG_LOG_HDR(lvl_name, color_n)                    \
    fm_kprintf("[" lvl_name "/" DBG_SECTION_NAME "] ")
#define _DBG_LOG_X_END                                     \
    fm_kprintf("\r\n")
#endif /* DBG_COLOR */

#define dbg_log_line(lvl, color_n, fmt, ...)                \
    do                                                      \
    {                                                       \
        _DBG_LOG_HDR(lvl, color_n);                         \
        fm_kprintf(fmt, ##__VA_ARGS__);                     \
        _DBG_LOG_X_END;                                     \
    }                                                       \
    while (0)

#define dbg_log_raw(...)         fm_kprintf(__VA_ARGS__);

#if (DBG_LEVEL >= DBG_LOG)
#define log_d(fmt, ...)      dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#else
#define log_d(...)
#endif

#if (DBG_LEVEL >= DBG_INFO)
#define log_i(fmt, ...)      dbg_log_line("I", 32, fmt, ##__VA_ARGS__)
#else
#define log_i(...)
#endif

#if (DBG_LEVEL >= DBG_WARNING)
#define log_w(fmt, ...)      dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#else
#define log_w(...)
#endif

#if (DBG_LEVEL >= DBG_ERROR)
#define log_e(fmt, ...)      dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define log_e(...)
#endif

#define log_raw(...)         dbg_log_raw(__VA_ARGS__)

int fm_kprintf(const char *fmt, ...);
int fm_putchar(uint8_t c);
int fm_getchar(uint8_t *c);

#endif /* DBG_ENABLE */

#endif /* __FM_LOG_H__ */
