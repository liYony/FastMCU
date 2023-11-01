#include <fm_section.h>

#if defined(SECTION_ENABLE)

#if defined(FM_SECTION_LOG)
#define DBG_LVL DBG_LOG
#include <fm_log.h>
FM_SECTION_DEF_FLASH(fmcu_polling, fm_dbg_sec_t *);
FM_SECTION_DEF_FLASH(fmcu_initlv0, fm_dbg_sec_t *);
FM_SECTION_DEF_FLASH(fmcu_initlv1, fm_dbg_sec_t *);
FM_SECTION_DEF_FLASH(fmcu_initlv2, fm_dbg_sec_t *);
FM_SECTION_DEF_FLASH(fmcu_initlv3, fm_dbg_sec_t *);
FM_SECTION_DEF_FLASH(fmcu_initlv4, fm_dbg_sec_t *);
#define _SECTION_EXEC(s_fn)                                 \
    FM_SECTION_FOR_EACH(fmcu_##s_fn, fm_dbg_sec_t *, s_fn)  \
    {                                                       \
        log_raw("<%s> [%s]\r\n", #s_fn, (*s_fn)->fn_name); \
        (*s_fn)->fn();                                      \
    }
#else
FM_SECTION_DEF_FLASH(fmcu_polling, fmcu_func_t);
FM_SECTION_DEF_FLASH(fmcu_initlv0, fmcu_func_t);
FM_SECTION_DEF_FLASH(fmcu_initlv1, fmcu_func_t);
FM_SECTION_DEF_FLASH(fmcu_initlv2, fmcu_func_t);
FM_SECTION_DEF_FLASH(fmcu_initlv3, fmcu_func_t);
FM_SECTION_DEF_FLASH(fmcu_initlv4, fmcu_func_t);
#define _SECTION_EXEC(fn)                           \
    FM_SECTION_FOR_EACH(fmcu_##fn, fmcu_func_t, fn) \
    {                                               \
        (*fn)();                                    \
    }
#endif

void fm_section_init(void)
{
    _SECTION_EXEC(initlv0);
    _SECTION_EXEC(initlv1);
    _SECTION_EXEC(initlv2);
    _SECTION_EXEC(initlv3);
    _SECTION_EXEC(initlv4);
}

void fm_section_exec(void)
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

#endif /* SECTION_ENABLE */
