/*
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-04-28     liYony       add platform resource
 */

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <fmdef.h>

typedef fm_ubase_t fm_resource_size_t;

struct fm_resource
{
    fm_resource_size_t start;
    fm_resource_size_t end;
    const char *name;
    unsigned long flags;
    struct fm_resource *parent, *sibling, *child;
};

/*
 * IO resources have these defined flags.
 */
#define FM_IORESOURCE_BITS 0x000000ff /* Bus-specific bits */

#define FM_IORESOURCE_TYPE_BITS 0x00001f00 /* Resource type */
#define FM_IORESOURCE_IO 0x00000100        /* PCI/ISA I/O ports */
#define FM_IORESOURCE_MEM 0x00000200
#define FM_IORESOURCE_REG 0x00000300 /* Register offsets */
#define FM_IORESOURCE_IRQ 0x00000400
#define FM_IORESOURCE_DMA 0x00000800
#define FM_IORESOURCE_BUS 0x00001000

#define FM_IORESOURCE_PREFETCH 0x00002000 /* No side effects */
#define FM_IORESOURCE_READONLY 0x00004000
#define FM_IORESOURCE_CACHEABLE 0x00008000
#define FM_IORESOURCE_RANGELENGTH 0x00010000
#define FM_IORESOURCE_SHADOWABLE 0x00020000

#define FM_IORESOURCE_SIZEALIGN 0x00040000  /* size indicates alignment */
#define FM_IORESOURCE_STARTALIGN 0x00080000 /* start field is alignment */

#define FM_IORESOURCE_MEM_64 0x00100000
#define FM_IORESOURCE_WINDOW 0x00200000 /* forwarded by bridge */
#define FM_IORESOURCE_MUXED 0x00400000  /* Resource is software muxed */

#define FM_IORESOURCE_EXCLUSIVE 0x08000000 /* Userland may not map this resource */
#define FM_IORESOURCE_DISABLED 0x10000000
#define FM_IORESOURCE_UNSET 0x20000000 /* No address assigned yet */
#define FM_IORESOURCE_AUTO 0x40000000
#define FM_IORESOURCE_BUSY 0x80000000 /* Driver has marked this resource busy */

/* PnP IRQ specific bits (FM_IORESOURCE_BITS) */
#define FM_IORESOURCE_IRQ_HIGHEDGE (1 << 0)
#define FM_IORESOURCE_IRQ_LOWEDGE (1 << 1)
#define FM_IORESOURCE_IRQ_HIGHLEVEL (1 << 2)
#define FM_IORESOURCE_IRQ_LOWLEVEL (1 << 3)
#define FM_IORESOURCE_IRQ_SHAREABLE (1 << 4)
#define FM_IORESOURCE_IRQ_OPTIONAL (1 << 5)

/* PnP DMA specific bits (FM_IORESOURCE_BITS) */
#define FM_IORESOURCE_DMA_TYPE_MASK (3 << 0)
#define FM_IORESOURCE_DMA_8BIT (0 << 0)
#define FM_IORESOURCE_DMA_8AND16BIT (1 << 0)
#define FM_IORESOURCE_DMA_16BIT (2 << 0)

#define FM_IORESOURCE_DMA_MASTER (1 << 2)
#define FM_IORESOURCE_DMA_BYTE (1 << 3)
#define FM_IORESOURCE_DMA_WORD (1 << 4)

#define FM_IORESOURCE_DMA_SPEED_MASK (3 << 6)
#define FM_IORESOURCE_DMA_COMPATIBLE (0 << 6)
#define FM_IORESOURCE_DMA_TYPEA (1 << 6)
#define FM_IORESOURCE_DMA_TYPEB (2 << 6)
#define FM_IORESOURCE_DMA_TYPEF (3 << 6)

/* PnP memory I/O specific bits (FM_IORESOURCE_BITS) */
#define FM_IORESOURCE_MEM_WRITEABLE (1 << 0)   /* dup: FM_IORESOURCE_READONLY */
#define FM_IORESOURCE_MEM_CACHEABLE (1 << 1)   /* dup: FM_IORESOURCE_CACHEABLE */
#define FM_IORESOURCE_MEM_RANGELENGTH (1 << 2) /* dup: FM_IORESOURCE_RANGELENGTH */
#define FM_IORESOURCE_MEM_TYPE_MASK (3 << 3)
#define FM_IORESOURCE_MEM_8BIT (0 << 3)
#define FM_IORESOURCE_MEM_16BIT (1 << 3)
#define FM_IORESOURCE_MEM_8AND16BIT (2 << 3)
#define FM_IORESOURCE_MEM_32BIT (3 << 3)
#define FM_IORESOURCE_MEM_SHADOWABLE (1 << 5) /* dup: FM_IORESOURCE_SHADOWABLE */
#define FM_IORESOURCE_MEM_EXPANSIONROM (1 << 6)

/* PnP I/O specific bits (FM_IORESOURCE_BITS) */
#define FM_IORESOURCE_IO_16BIT_ADDR (1 << 0)
#define FM_IORESOURCE_IO_FIXED (1 << 1)

/* PCI ROM control bits (FM_IORESOURCE_BITS) */
#define FM_IORESOURCE_ROM_ENABLE (1 << 0)    /* ROM is enabled, same as PCI_ROM_ADDRESS_ENABLE */
#define FM_IORESOURCE_ROM_SHADOW (1 << 1)    /* ROM is copy at C000:0 */
#define FM_IORESOURCE_ROM_COPY (1 << 2)      /* ROM is alloc'd copy, resource field overlaid */
#define FM_IORESOURCE_ROM_BIOS_COPY (1 << 3) /* ROM is BIOS copy, resource field overlaid */

/* PCI control bits.  Shares FM_IORESOURCE_BITS with above PCI ROM.  */
#define FM_IORESOURCE_PCI_FIXED (1 << 4) /* Do not move resource */

/* helpers to define resources */
#define DEFINE_RES_NAMED(_start, _size, _name, _flags) \
    {                                                  \
        .start = (_start),                             \
        .end = (_start) + (_size)-1,                   \
        .name = (_name),                               \
        .flags = (_flags),                             \
    }

#define DEFINE_RES_IO_NAMED(_start, _size, _name) \
    DEFINE_RES_NAMED((_start), (_size), (_name), FM_IORESOURCE_IO)
#define DEFINE_RES_IO(_start, _size) \
    DEFINE_RES_IO_NAMED((_start), (_size), NULL)

#define DEFINE_RES_MEM_NAMED(_start, _size, _name) \
    DEFINE_RES_NAMED((_start), (_size), (_name), FM_IORESOURCE_MEM)
#define DEFINE_RES_MEM(_start, _size) \
    DEFINE_RES_MEM_NAMED((_start), (_size), NULL)

#define DEFINE_RES_IRQ_NAMED(_irq, _name) \
    DEFINE_RES_NAMED((_irq), 1, (_name), FM_IORESOURCE_IRQ)
#define DEFINE_RES_IRQ(_irq) \
    DEFINE_RES_IRQ_NAMED((_irq), NULL)

#define DEFINE_RES_DMA_NAMED(_dma, _name) \
    DEFINE_RES_NAMED((_dma), 1, (_name), FM_IORESOURCE_DMA)
#define DEFINE_RES_DMA(_dma) \
    DEFINE_RES_DMA_NAMED((_dma), NULL)

fm_inline unsigned long fm_resource_type(const struct fm_resource *res)
{
	return res->flags & FM_IORESOURCE_TYPE_BITS;
}

#endif // !__RESOURCE_H__
