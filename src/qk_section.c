#include <qk_section.h>

#if defined(QK_SECTION_LOG)
#define LOG_LVL LVL_DEBUG
#include <qk_log.h>
QK_SECTION_DEF_FLASH(qkit_polling, qk_dbg_sec_t *);
QK_SECTION_DEF_FLASH(qkit_initlv0, qk_dbg_sec_t *);
QK_SECTION_DEF_FLASH(qkit_initlv1, qk_dbg_sec_t *);
QK_SECTION_DEF_FLASH(qkit_initlv2, qk_dbg_sec_t *);
QK_SECTION_DEF_FLASH(qkit_initlv3, qk_dbg_sec_t *);
QK_SECTION_DEF_FLASH(qkit_initlv4, qk_dbg_sec_t *);
#define _SECTION_EXEC(s_fn)                                \
    QK_SECTION_FOR_EACH(qkit_##s_fn, qk_dbg_sec_t *, s_fn) \
    {                                                      \
        log_d("<sec>%s.%s\r\n", #s_fn, (*s_fn)->fn_name);  \
        (*s_fn)->fn();                                     \
    }
#else
QK_SECTION_DEF_FLASH(qkit_polling, qkit_func_t);
QK_SECTION_DEF_FLASH(qkit_initlv0, qkit_func_t);
QK_SECTION_DEF_FLASH(qkit_initlv1, qkit_func_t);
QK_SECTION_DEF_FLASH(qkit_initlv2, qkit_func_t);
QK_SECTION_DEF_FLASH(qkit_initlv3, qkit_func_t);
QK_SECTION_DEF_FLASH(qkit_initlv4, qkit_func_t);
#define _SECTION_EXEC(fn)                           \
    QK_SECTION_FOR_EACH(qkit_##fn, qkit_func_t, fn) \
    {                                               \
        (*fn)();                                    \
    }
#endif

void qk_init(void)
{
    _SECTION_EXEC(initlv0);
    _SECTION_EXEC(initlv1);
    _SECTION_EXEC(initlv2);
    _SECTION_EXEC(initlv3);
    _SECTION_EXEC(initlv4);
}

void qk_exec(void)
{
    _SECTION_EXEC(polling);
}

void initlv0_func(void) {}
void initlv1_func(void) {}
void initlv2_func(void) {}
void initlv3_func(void) {}
void initlv4_func(void) {}
void polling_func(void) {}

INITLV0_EXPORT(initlv0_func);
INITLV1_EXPORT(initlv1_func);
INITLV2_EXPORT(initlv2_func);
INITLV3_EXPORT(initlv3_func);
INITLV4_EXPORT(initlv4_func);
POLLING_EXPORT(polling_func);
