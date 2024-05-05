/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-6-27      GuEe-GUI     first version
 * 2024-05-05     liYony       Adaptive FastMCU
 */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <fmdef.h>

typedef fm_ubase_t fm_bitmap_t;

#define FM_BITMAP_BITS_MIN              (sizeof(fm_bitmap_t) * 8)
#define FM_BITMAP_LEN(bits)             (((bits) + (FM_BITMAP_BITS_MIN) - 1) / (FM_BITMAP_BITS_MIN))
#define FM_BITMAP_BIT_LEN(nr)           (nr * FM_BITMAP_BITS_MIN)
#define FM_BITMAP_DECLARE(name, bits)   fm_bitmap_t name[FM_BITMAP_LEN(bits)]

fm_inline void fm_bitmap_set_bit(fm_bitmap_t *bitmap, fm_uint32_t bit)
{
    bitmap[bit / FM_BITMAP_BITS_MIN] |= (1UL << (bit & (FM_BITMAP_BITS_MIN - 1)));
}

fm_inline fm_bool_t fm_bitmap_test_bit(fm_bitmap_t *bitmap, fm_uint32_t bit)
{
    return !!(bitmap[bit / FM_BITMAP_BITS_MIN] & (1UL << (bit & (FM_BITMAP_BITS_MIN - 1))));
}

fm_inline void fm_bitmap_clear_bit(fm_bitmap_t *bitmap, fm_uint32_t bit)
{
    bitmap[bit / FM_BITMAP_BITS_MIN] &= ~(1UL << (bit & (FM_BITMAP_BITS_MIN - 1)));
}

fm_inline fm_size_t fm_bitmap_next_set_bit(fm_bitmap_t *bitmap, fm_size_t start, fm_size_t limit)
{
    fm_size_t bit;

    for (bit = start; bit < limit && !fm_bitmap_test_bit(bitmap, bit); ++bit)
    {
    }

    return bit;
}

fm_inline fm_size_t fm_bitmap_next_clear_bit(fm_bitmap_t *bitmap, fm_size_t start, fm_size_t limit)
{
    fm_size_t bit;

    for (bit = start; bit < limit && fm_bitmap_test_bit(bitmap, bit); ++bit)
    {
    }

    return bit;
}

#define fm_bitmap_for_each_bit_from(state, bitmap, from, bit, limit)        \
    for ((bit) = fm_bitmap_next_##state##_bit((bitmap), (from), (limit));   \
         (bit) < (limit);                                                   \
         (bit) = fm_bitmap_next_##state##_bit((bitmap), (bit + 1), (limit)))

#define fm_bitmap_for_each_set_bit_from(bitmap, from, bit, limit) \
    fm_bitmap_for_each_bit_from(set, bitmap, from, bit, limit)

#define fm_bitmap_for_each_set_bit(bitmap, bit, limit) \
    fm_bitmap_for_each_set_bit_from(bitmap, 0, bit, limit)

#define fm_bitmap_for_each_clear_bit_from(bitmap, from, bit, limit) \
    fm_bitmap_for_each_bit_from(clear, bitmap, from, bit, limit)

#define fm_bitmap_for_each_clear_bit(bitmap, bit, limit) \
    fm_bitmap_for_each_clear_bit_from(bitmap, 0, bit, limit)

#endif /* __BITMAP_H__ */
