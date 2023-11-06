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
#define __STATIC_INLINE static inline
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define __WEAKDEF __attribute__((weak))
#define __STATIC_INLINE static __inline
#elif defined(__GNUC__)
#define __WEAKDEF __attribute__((weak))
#define __STATIC_INLINE static __inline
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

#define SIZE_ALIGNMENT(size, align) (((size) + (align)-1) & ~((align)-1))

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
