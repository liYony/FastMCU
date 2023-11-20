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

/* boolean type definitions */
#define FM_TRUE                         1               /**< boolean true  */
#define FM_FALSE                        0               /**< boolean fails */

/* null pointer definition */
#define FM_NULL                         0

/* FastMCU error code definitions (copy from RT-Thread) */
#define FM_EOK                          0               /**< There is no error */
#define FM_ERROR                        1               /**< A generic error happens */
#define FM_ETIMEOUT                     2               /**< Timed out */
#define FM_EFULL                        3               /**< The resource is full */
#define FM_EEMPTY                       4               /**< The resource is empty */
#define FM_ENOMEM                       5               /**< No memory */
#define FM_ENOSYS                       6               /**< No system */
#define FM_EBUSY                        7               /**< Busy */
#define FM_EIO                          8               /**< IO error */
#define FM_EINTR                        9               /**< Interrupted system call */
#define FM_EINVAL                       10              /**< Invalid argument */
#define FM_ETRAP                        11              /**< Trap event */
#define FM_ENOENT                       12              /**< No entry */
#define FM_ENOSPC                       13              /**< No space left */
#define FM_EPERM                        14              /**< Operation not permitted */

#define FM_ASSERT(EX)

/**
 * fm_container_of - return the start address of struct type, while ptr is the
 * member of struct type.
 */
#define fm_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

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
