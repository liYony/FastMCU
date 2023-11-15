#ifndef __FM_TYPE_H__
#define __FM_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if defined(__CC_ARM)
#define __WEAKDEF __attribute__((weak))
#define __STATIC_INLINE static __inline
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define __WEAKDEF __attribute__((weak))
#define __STATIC_INLINE static __inline
#elif defined(__GNUC__)
#define __WEAKDEF __attribute__((weak))
#define __STATIC_INLINE static inline
#elif defined(__ICCARM__)
#define __STATIC_INLINE static inline
#if __ICCARM_V8
#define __WEAKDEF __attribute__((weak))
#else
#define __WEAKDEF _Pragma("__weak")
#endif
#else
#define __WEAKDEF __attribute__((weak))
#endif

/**
 * @def FM_IS_ALIGN(addr, align)
 * Return true(1) or false(0).
 *     FM_IS_ALIGN(128, 4) is judging whether 128 aligns with 4.
 *     The result is 1, which means 128 aligns with 4.
 * @note If the address is NULL, false(0) will be returned
 */
#define FM_IS_ALIGN(addr, align) ((!(addr & (align - 1))) && (addr != NULL))

/**
 * @def FM_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. FM_ALIGN(13, 4)
 * would return 16.
 */
#define FM_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @def FM_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. FM_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define FM_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define OFFSET_OF(type, member) ((uint32_t)(&((type *)0)->member))

#define SAFE_INVOKE(f, ...)   \
    do                        \
    {                         \
        if (f != NULL)        \
        {                     \
            f(__VA_ARGS__);   \
        }                     \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __FM_TYPE_H__ */
