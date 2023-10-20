#ifndef __QK_SECTION_H__
#define __QK_SECTION_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <qkconfig.h>

//#define QK_SECTION_LOG

#if defined(QK_SECTION_LOG)
typedef void (*qkit_func_t)(void);
typedef struct qk_dbg_sec
{
    const char *fn_name;
    const qkit_func_t fn;
} qk_dbg_sec_t;
#else
typedef void (*qkit_func_t)(void);
#endif

#if defined(__ICCARM__)
#pragma language = extended
#endif

#define CONCAT_2(s1, s2) s1##s2
#define __stringify(x...) #x
#define STRINGIFY(s1) __stringify(s1)

/**
 * \brief the beginning of a section
 */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000))
#define _SECTION_START_ADDR(section_name) &CONCAT_2(section_name, $$Base)

#elif defined(__GNUC__)
#define _SECTION_START_ADDR(section_name) &CONCAT_2(__start_, section_name)

#elif defined(__ICCARM__)
#define _SECTION_START_ADDR(section_name) __section_begin(STRINGIFY(section_name))
#endif

/**
 * \brief the end of a section.
 */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000))
#define _SECTION_END_ADDR(section_name) &CONCAT_2(section_name, $$Limit)

#elif defined(__GNUC__)
#define _SECTION_END_ADDR(section_name) &CONCAT_2(__stop_, section_name)

#elif defined(__ICCARM__)
#define _SECTION_END_ADDR(section_name) __section_end(STRINGIFY(section_name))
#endif

/**
 * \brief the length of a given section
 */
#define _SECTION_LENGTH(section_name) \
    ((size_t)_SECTION_END_ADDR(section_name) - (size_t)_SECTION_START_ADDR(section_name))

/**
 * \brief creating a section
 */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000))
#define _SECTION_DEF(section_name, data_type)         \
    extern data_type *CONCAT_2(section_name, $$Base); \
    extern void *CONCAT_2(section_name, $$Limit)

#elif defined(__GNUC__)
#define _SECTION_DEF(section_name, data_type)           \
    extern data_type *CONCAT_2(__start_, section_name); \
    extern void *CONCAT_2(__stop_, section_name)

#elif defined(__ICCARM__)
#define _SECTION_DEF(section_name, data_type) \
    _Pragma(STRINGIFY(section = STRINGIFY(section_name)));

#endif

/**
 * \brief declaring a variable and registering it in a section.
 */
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6000000))
#define _SECTION_ITEM_REGISTER(section_name, section_var) \
    section_var __attribute__((section(STRINGIFY(section_name)))) __attribute__((used))

#elif defined(__GNUC__)
#define _SECTION_ITEM_REGISTER(section_name, section_var) \
    section_var __attribute__((section("." STRINGIFY(section_name)))) __attribute__((used))

#elif defined(__ICCARM__)
#define _SECTION_ITEM_REGISTER(section_name, section_var) \
    __root section_var @STRINGIFY(section_name)
#endif

/**
 * \brief retrieving a variable from a section.
 */
#define _SECTION_ITEM_GET(section_name, data_type, i) \
    ((data_type *)_SECTION_START_ADDR(section_name) + (i))

/**
 * \brief getting the number of variables in a section.
 */
#define _SECTION_ITEM_COUNT(section_name, data_type) \
    _SECTION_LENGTH(section_name) / sizeof(data_type)

/**
 * \brief define a section in flash or ram
 */
#define QK_SECTION_DEF_FLASH(section_name, data_type) _SECTION_DEF(section_name, const data_type)
#define QK_SECTION_DEF_RAM(section_name, data_type) _SECTION_DEF(section_name, data_type)

/**
 * \brief register item in flash or ram section
 */
#define QK_SECTION_ITEM_REGISTER_FLASH(section_name, data_type, var_name) \
    _SECTION_ITEM_REGISTER(section_name, const data_type var_name)
#define QK_SECTION_ITEM_REGISTER_RAM(section_name, data_type, var_name) \
    _SECTION_ITEM_REGISTER(section_name, data_type var_name)

/**
 * \brief Traverse a section
 */
#define QK_SECTION_FOR_EACH(section_name, data_type, variable)     \
    data_type *variable;                                           \
    for (variable = _SECTION_ITEM_GET(section_name, data_type, 0); \
         (intptr_t)variable != (intptr_t)_SECTION_END_ADDR(section_name); variable++)

/**
 * \brief polling and initialize the function to execute.
 */
#if defined(QK_SECTION_LOG)
#define POLLING_EXPORT(func)                \
    qk_dbg_sec_t dbg##func = {#func, func}; \
    QK_SECTION_ITEM_REGISTER_FLASH(qkit_polling, qk_dbg_sec_t *, CONCAT_2(qkit_polling_, func)) = &dbg##func
#define INITLVL_EXPORT(func, lvl)           \
    qk_dbg_sec_t dbg##func = {#func, func}; \
    QK_SECTION_ITEM_REGISTER_FLASH(qkit_initlv##lvl, qk_dbg_sec_t *, CONCAT_2(qkit_initlv##lvl##_, func)) = &dbg##func
#else
#define POLLING_EXPORT(func) \
    QK_SECTION_ITEM_REGISTER_FLASH(qkit_polling, qkit_func_t, CONCAT_2(qkit_polling_, func)) = func
#define INITLVL_EXPORT(func, lvl) \
    QK_SECTION_ITEM_REGISTER_FLASH(qkit_initlv##lvl, qkit_func_t, CONCAT_2(qkit_initlv##lvl##_, func)) = func
#endif

/**
 * \brief initializes the function to execute.
 */
#define INITLV0_EXPORT(func) INITLVL_EXPORT(func, 0)
#define INITLV1_EXPORT(func) INITLVL_EXPORT(func, 1)
#define INITLV2_EXPORT(func) INITLVL_EXPORT(func, 2)
#define INITLV3_EXPORT(func) INITLVL_EXPORT(func, 3)
#define INITLV4_EXPORT(func) INITLVL_EXPORT(func, 4)

void qk_init(void);
void qk_exec(void);

#ifdef __cplusplus
}
#endif

#endif /* __QK_SECTION_H__ */
