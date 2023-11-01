#ifndef __QK_LOG_H__
#define __QK_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <fmconfig.h>

#define LVL_INFO        (1)
#define LVL_WARNING     (2)
#define LVL_ERROR       (3)
#define LVL_DEBUG       (4)

#ifndef LOG_BUF_SIZE
#define LOG_BUF_SIZE (512)
#endif

#define _LOG_BUF_SIZE LOG_BUF_SIZE

#if !defined(LOG_LVL)
#if defined(LOG_LVL_DEBUG)
#define LOG_LVL LVL_DEBUG
#endif /* LOG_LVL_DEBUG */
#if defined(LOG_LVL_INFO)
#define LOG_LVL LVL_INFO
#endif /* LOG_LVL_INFO */
#if defined(LOG_LVL_WARNING)
#define LOG_LVL LVL_WARNING
#endif /* LOG_LVL_WARNING */
#if defined(LOG_LVL_ERROR)
#define LOG_LVL LVL_ERROR
#endif /* LOG_LVL_ERROR */
#endif /* !LOG_LVL */

#define _LOG_PARAM_NULL 0X0
#define _LOG_PARAM_LINE 0X1
#define _LOG_PARAM_FUNC 0X2
#define _LOG_PARAM_FILE 0X4

#define _LOG_D_PARAM (_LOG_PARAM_NULL)
#define _LOG_I_PARAM (_LOG_PARAM_FILE)
#define _LOG_W_PARAM (_LOG_PARAM_FILE | _LOG_PARAM_FUNC)
#define _LOG_E_PARAM (_LOG_PARAM_FILE | _LOG_PARAM_FUNC | _LOG_PARAM_LINE)

#define _LOG_PARAM_DEFAULT __FILE__, __func__, __LINE__

#if (LOG_LVL == LVL_INFO)

#define log_i(...) qk_log_out(0, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_w(...) qk_log_out(1, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_e(...) qk_log_out(2, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_d(...)

#elif (LOG_LVL == LVL_WARNING)

#define log_i(...)
#define log_w(...) qk_log_out(1, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_e(...) qk_log_out(2, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_d(...)

#elif (LOG_LVL == LVL_ERROR)

#define log_i(...)
#define log_w(...)
#define log_e(...) qk_log_out(2, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_d(...)

#elif (LOG_LVL == LVL_DEBUG)

#define log_i(...) qk_log_out(0, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_w(...) qk_log_out(1, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_e(...) qk_log_out(2, _LOG_PARAM_DEFAULT, __VA_ARGS__)
#define log_d(...) qk_log_out(3, _LOG_PARAM_DEFAULT, __VA_ARGS__)

#else

#define log_i(...)
#define log_w(...)
#define log_e(...)
#define log_d(...)

#endif

#undef LOG_LVL

void qk_log_out(uint8_t type, const char *ptr_file, const char *ptr_func, uint32_t line,
             const char *fmt, ...);
void qk_log_output(void *p);
int qk_kprintf(const char *fmt, ...);
#ifdef __cplusplus
}
#endif

#endif /* __QK_LOG_H__ */
