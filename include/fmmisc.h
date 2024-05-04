/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-25     GuEe-GUI     the first version
 * 2024-05-04     liYony       Adaptive FastMCU
 */

#ifndef __FM_MISC_H__
#define __FM_MISC_H__

#include <fmdef.h>

#ifdef ARCH_CPU_64BIT
#define FM_BITS_PER_LONG 64
#else
#define FM_BITS_PER_LONG 32
#endif
#define FM_BITS_PER_LONG_LONG 64

#define FM_DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))

#define FM_DIV_ROUND_CLOSEST(x, divisor)        \
({                                              \
    typeof(x) __x = x;                          \
    typeof(divisor) __d = divisor;              \
    (((typeof(x))-1) > 0 ||                     \
     ((typeof(divisor))-1) > 0 ||               \
     (((__x) > 0) == ((__d) > 0))) ?            \
            (((__x) + ((__d) / 2)) / (__d)) :   \
            (((__x) - ((__d) / 2)) / (__d));    \
})

#define FM_BIT(n)               (1UL << (n))
#define FM_BIT_ULL(n)           (1ULL << (n))
#define FM_BIT_MASK(nr)         (1UL << ((nr) % FM_BITS_PER_LONG))
#define FM_BIT_WORD(nr)         ((nr) / FM_BITS_PER_LONG)

#define FM_BITS_PER_BYTE        8
#define FM_BITS_PER_TYPE(type)  (sizeof(type) * FM_BITS_PER_BYTE)
#define FM_BITS_TO_BYTES(nr)    FM_DIV_ROUND_UP(nr, FM_BITS_PER_TYPE(char))
#define FM_BITS_TO_LONGS(nr)    FM_DIV_ROUND_UP(nr, FM_BITS_PER_TYPE(long))

#define FM_GENMASK(h, l)        (((~0UL) << (l)) & (~0UL >> (FM_BITS_PER_LONG - 1 - (h))))
#define FM_GENMASK_ULL(h, l)    (((~0ULL) << (l)) & (~0ULL >> (FM_BITS_PER_LONG_LONG - 1 - (h))))

#define FM_ARRAY_SIZE(arr)      (sizeof(arr) / sizeof(arr[0]))

#define fm_min(x, y)            \
({                              \
    typeof(x) _x = (x);         \
    typeof(y) _y = (y);         \
    (void) (&_x == &_y);        \
    _x < _y ? _x : _y;          \
})

#define fm_max(x, y)            \
({                              \
    typeof(x) _x = (x);         \
    typeof(y) _y = (y);         \
    (void) (&_x == &_y);        \
    _x > _y ? _x : _y;          \
})

#define fm_min_t(type, x, y)    \
({                              \
    type _x = (x);              \
    type _y = (y);              \
    _x < _y ? _x: _y;           \
})

#define fm_clamp(val, lo, hi)   fm_min((typeof(val))fm_max(val, lo), hi)

#define fm_do_div(n, base)              \
({                                      \
    fm_uint32_t _base = (base), _rem;   \
    _rem = ((fm_uint64_t)(n)) % _base;  \
    (n) = ((fm_uint64_t)(n)) / _base;   \
    if (_rem > _base / 2)               \
        ++(n);                          \
    _rem;                               \
})

#endif /* __FM_MISC_H__ */
