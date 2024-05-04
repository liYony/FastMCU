/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-24     GuEe-GUI     first version
 * 2024-05-04     liYony       Adaptive FastMCU
 */

#ifndef __PIC_H__
#define __PIC_H__

#include <fmcu.h>

struct fm_pic_ops;
struct fm_pic_irq;

struct fm_pic
{
    /*
     * Other IC is not implemented with PIC but fm_device/object, we need to
     * identify with this object:
     *
     *  struct fm_ic_XYZ_device
     *  {
     *      struct fm_device parent;
     *      struct fm_pic pic;
     *      ...
     *  };
     */
    struct fm_object parent;

    fm_list_t list;

    struct fm_pic_ops *ops;

    void *priv_data;
    void *user_data;

    int irq_start;
    fm_size_t irq_nr;
    struct fm_pic_irq *pirqs;
};

struct fm_pic_ops
{
    const char  *name;

    fm_err_t    (*irq_init)(struct fm_pic *pic);
    fm_err_t    (*irq_finit)(struct fm_pic *pic);

    void        (*irq_enable)(struct fm_pic_irq *pirq);
    void        (*irq_disable)(struct fm_pic_irq *pirq);
    void        (*irq_ack)(struct fm_pic_irq *pirq);
    void        (*irq_mask)(struct fm_pic_irq *pirq);
    void        (*irq_unmask)(struct fm_pic_irq *pirq);
    void        (*irq_eoi)(struct fm_pic_irq *pirq);

    fm_err_t    (*irq_set_priority)(struct fm_pic_irq *pirq, fm_uint32_t priority);
    fm_err_t    (*irq_set_triger_mode)(struct fm_pic_irq *pirq, fm_uint32_t mode);

#define FM_PIC_F_IRQ_ROUTING    FM_BIT(0)   /* Routing ISR when cascade */
    fm_ubase_t  flags;
};

struct fm_pic_isr
{
    fm_list_t list;

#define FM_IRQ_F_NONE 0
    int flags;
    struct fm_irq_desc action;
};

struct fm_pic_irq
{
    int irq;
    int hwirq;

#define FM_IRQ_MODE_NONE            0
#define FM_IRQ_MODE_EDGE_RISING     1
#define FM_IRQ_MODE_EDGE_FALLING    2
#define FM_IRQ_MODE_EDGE_BOTH       (FM_IRQ_MODE_EDGE_FALLING | FM_IRQ_MODE_EDGE_RISING)
#define FM_IRQ_MODE_LEVEL_HIGH      4
#define FM_IRQ_MODE_LEVEL_LOW       8
#define FM_IRQ_MODE_LEVEL_MASK      (FM_IRQ_MODE_LEVEL_LOW | FM_IRQ_MODE_LEVEL_HIGH)
#define FM_IRQ_MODE_MASK            0xf
    fm_uint32_t mode;

    fm_uint32_t priority;

    fm_list_t list;
    fm_list_t children_nodes;

    struct fm_pic_isr isr;

    struct fm_pic *pic;
    struct fm_pic_irq *parent;
};

void fm_pic_default_name(struct fm_pic *pic);
fm_err_t fm_pic_linear_irq(struct fm_pic *pic, fm_size_t irq_nr);
int fm_pic_config_irq(struct fm_pic *pic, int irq_index, int hwirq);
struct fm_pic_irq *fm_pic_find_pirq(struct fm_pic *pic, int irq);
fm_err_t fm_pic_cascade(struct fm_pic_irq *pirq, int parent_irq);
fm_err_t fm_pic_uncascade(struct fm_pic_irq *pirq);
fm_err_t fm_pic_attach_irq(int irq, fm_isr_handler_t handler, void *uid, const char *name, int flags);
fm_err_t fm_pic_detach_irq(int irq, void *uid);
fm_err_t fm_pic_add_traps(fm_bool_t (*handler)(void *), void *data);
fm_err_t fm_pic_do_traps(void);
fm_err_t fm_pic_handle_isr(struct fm_pic_irq *pirq);
fm_err_t fm_pic_user_extends(struct fm_pic *pic);
fm_err_t fm_pic_irq_init(void);
fm_err_t fm_pic_irq_finit(void);
void fm_pic_irq_enable(int irq);
void fm_pic_irq_disable(int irq);
void fm_pic_irq_ack(int irq);
void fm_pic_irq_mask(int irq);
void fm_pic_irq_unmask(int irq);
void fm_pic_irq_eoi(int irq);
fm_err_t fm_pic_irq_set_priority(int irq, fm_uint32_t priority);
fm_uint32_t fm_pic_irq_get_priority(int irq);
fm_err_t fm_pic_irq_set_triger_mode(int irq, fm_uint32_t mode);
fm_uint32_t fm_pic_irq_get_triger_mode(int irq);
void fm_pic_irq_parent_enable(struct fm_pic_irq *pirq);
void fm_pic_irq_parent_disable(struct fm_pic_irq *pirq);
void fm_pic_irq_parent_ack(struct fm_pic_irq *pirq);
void fm_pic_irq_parent_mask(struct fm_pic_irq *pirq);
void fm_pic_irq_parent_unmask(struct fm_pic_irq *pirq);
void fm_pic_irq_parent_eoi(struct fm_pic_irq *pirq);
fm_err_t fm_pic_irq_parent_set_priority(struct fm_pic_irq *pirq, fm_uint32_t priority);
fm_err_t fm_pic_irq_parent_set_triger_mode(struct fm_pic_irq *pirq, fm_uint32_t mode);

fm_err_t fm_pic_init(void);

#endif /* __PIC_H__ */
