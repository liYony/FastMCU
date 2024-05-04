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

#include <fmdevice.h>

#define DBG_TAG "pic"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

struct irq_traps
{
    fm_list_t list;

    void *data;
    fm_bool_t (*handler)(void *);
};

static int _pirq_hash_idx = 0;

static struct fm_pic_irq _pirq_hash[FM_PIC_MAX_HANDLERS] =
{
    [0 ... FM_PIC_MAX_HANDLERS - 1] =
    {
        .irq = -1,
        .hwirq = -1,
        .mode = FM_IRQ_MODE_NONE,
        .priority = FM_UINT32_MAX,
    }
};

static fm_size_t _pic_name_max = sizeof("PIC");
static fm_list_t _pic_nodes = FM_LIST_OBJECT_INIT(_pic_nodes);
static fm_list_t _traps_nodes = FM_LIST_OBJECT_INIT(_traps_nodes);

static struct fm_pic_irq *irq2pirq(int irq)
{
    struct fm_pic_irq *pirq = FM_NULL;

    if ((irq >= 0) && (irq < FM_PIC_MAX_HANDLERS))
    {
        pirq = &_pirq_hash[irq];

        if (pirq->irq < 0)
        {
            pirq = FM_NULL;
        }
    }

    if (!pirq)
    {
        LOG_E("irq = %d is invalid", irq);
    }

    return pirq;
}

static void append_pic(struct fm_pic *pic)
{
    int pic_name_len = fm_strlen(pic->ops->name);

    fm_list_insert_before(&_pic_nodes, &pic->list);

    if (pic_name_len > _pic_name_max)
    {
        _pic_name_max = pic_name_len;
    }
}

void fm_pic_default_name(struct fm_pic *pic)
{
    if (pic)
    {
    #if FM_NAME_MAX > 0
        fm_strncpy(pic->parent.name, "PIC", FM_NAME_MAX);
    #else
        pic->parent.name = "PIC";
    #endif
    }
}

fm_err_t fm_pic_linear_irq(struct fm_pic *pic, fm_size_t irq_nr)
{
    fm_err_t err = FM_EOK;

    if (pic && pic->ops && pic->ops->name)
    {
        if (_pirq_hash_idx + irq_nr <= FM_ARRAY_SIZE(_pirq_hash))
        {
            fm_list_init(&pic->list);

            fm_pic_default_name(pic);
            pic->parent.type = FM_Object_Class_Unknown;

            pic->irq_start = _pirq_hash_idx;
            pic->irq_nr = irq_nr;
            pic->pirqs = &_pirq_hash[_pirq_hash_idx];

            _pirq_hash_idx += irq_nr;

            append_pic(pic);

            LOG_D("%s alloc irqs ranges [%d, %d]", pic->ops->name,
                    pic->irq_start, pic->irq_start + pic->irq_nr);
        }
        else
        {
            LOG_E("%s alloc %d irqs is overflow", pic->ops->name, irq_nr);

            err = -FM_EEMPTY;
        }
    }
    else
    {
        err = -FM_EINVAL;
    }

    return err;
}

static void config_pirq(struct fm_pic *pic, struct fm_pic_irq *pirq, int irq, int hwirq)
{
    pirq->irq = irq;
    pirq->hwirq = hwirq;

    pirq->pic = pic;

    fm_list_init(&pirq->list);
    fm_list_init(&pirq->children_nodes);
    fm_list_init(&pirq->isr.list);
}

int fm_pic_config_irq(struct fm_pic *pic, int irq_index, int hwirq)
{
    int irq;

    if (pic && hwirq >= 0)
    {
        irq = pic->irq_start + irq_index;

        if (irq >= 0 && irq < FM_PIC_MAX_HANDLERS)
        {
            config_pirq(pic, &_pirq_hash[irq], irq, hwirq);

            LOG_D("%s config %s %d to hwirq %d", pic->ops->name, "irq", irq, hwirq);
        }
        else
        {
            irq = -FM_ERROR;
        }
    }
    else
    {
        irq = -FM_EINVAL;
    }

    return irq;
}

struct fm_pic_irq *fm_pic_find_pirq(struct fm_pic *pic, int irq)
{
    if (pic && irq >= pic->irq_start && irq <= pic->irq_start + pic->irq_nr)
    {
        return &pic->pirqs[irq - pic->irq_start];
    }

    return FM_NULL;
}

fm_err_t fm_pic_cascade(struct fm_pic_irq *pirq, int parent_irq)
{
    fm_err_t err = FM_EOK;

    if (pirq && !pirq->parent && parent_irq >= 0)
    {
        struct fm_pic_irq *parent;

        parent = irq2pirq(parent_irq);

        if (parent)
        {
            pirq->parent = parent;
            pirq->priority = parent->priority;
        }

        if (parent && pirq->pic->ops->flags & FM_PIC_F_IRQ_ROUTING)
        {
            fm_list_insert_before(&parent->children_nodes, &pirq->list);
        }
    }
    else
    {
        err = -FM_EINVAL;
    }

    return err;
}

fm_err_t fm_pic_uncascade(struct fm_pic_irq *pirq)
{
    fm_err_t err = FM_EOK;

    if (pirq && pirq->parent)
    {
        struct fm_pic_irq *parent;

        parent = pirq->parent;
        pirq->parent = FM_NULL;

        if (parent && pirq->pic->ops->flags & FM_PIC_F_IRQ_ROUTING)
        {
            fm_list_remove(&pirq->list);
        }
    }
    else
    {
        err = -FM_EINVAL;
    }

    return err;
}

fm_err_t fm_pic_attach_irq(int irq, fm_isr_handler_t handler, void *uid, const char *name, int flags)
{
    fm_err_t err = -FM_EINVAL;
    struct fm_pic_irq *pirq;

    if (handler && name && (pirq = irq2pirq(irq)))
    {
        struct fm_pic_isr *isr = FM_NULL;

        err = FM_EOK;

        if (!pirq->isr.action.handler)
        {
            /* first attach */
            isr = &pirq->isr;
            fm_list_init(&isr->list);
        }
        else
        {
            if ((isr = fm_malloc(sizeof(*isr))))
            {
                fm_list_init(&isr->list);

                fm_list_insert_after(&pirq->isr.list, &isr->list);
            }
            else
            {
                LOG_E("No memory to save '%s' isr", name);
                err = -FM_ERROR;
            }
        }

        if (!err)
        {
            isr->flags = flags;
            isr->action.handler = handler;
            isr->action.param = uid;
        #ifdef FM_USING_INTERRUPT_INFO
            isr->action.counter = 0;
            fm_strncpy(isr->action.name, name, FM_NAME_MAX);
        #endif
        }
    }

    return err;
}

fm_err_t fm_pic_detach_irq(int irq, void *uid)
{
    fm_err_t err = -FM_EINVAL;
    struct fm_pic_irq *pirq = irq2pirq(irq);

    if (pirq)
    {
        fm_bool_t will_free = FM_FALSE;
        struct fm_pic_isr *isr = FM_NULL;

        isr = &pirq->isr;

        if (isr->action.param == uid)
        {
            if (fm_list_isempty(&isr->list))
            {
                isr->action.handler = FM_NULL;
                isr->action.param = FM_NULL;
            }
            else
            {
                struct fm_pic_isr *next_isr = fm_list_entry(isr->list.next, struct fm_pic_isr, list);

                fm_list_remove(&next_isr->list);

                isr->action.handler = next_isr->action.handler;
                isr->action.param = next_isr->action.param;
            #ifdef FM_USING_INTERRUPT_INFO
                isr->action.counter = next_isr->action.counter;
                fm_strncpy(isr->action.name, next_isr->action.name, FM_NAME_MAX);
            #endif

                isr = next_isr;
                will_free = FM_TRUE;
            }

            err = FM_EOK;
        }
        else
        {
            fm_list_for_each_entry(isr, &pirq->isr.list, list)
            {
                if (isr->action.param == uid)
                {
                    err = FM_EOK;

                    will_free = FM_TRUE;
                    fm_list_remove(&isr->list);
                    break;
                }
            }
        }

        if (will_free)
        {
            fm_free(isr);
        }
    }

    return err;
}

fm_err_t fm_pic_add_traps(fm_bool_t (*handler)(void *), void *data)
{
    fm_err_t err = -FM_EINVAL;

    if (handler)
    {
        struct irq_traps *traps = fm_malloc(sizeof(*traps));

        if (traps)
        {
            fm_list_init(&traps->list);

            traps->data = data;
            traps->handler = handler;

            fm_list_insert_before(&_traps_nodes, &traps->list);
            err = FM_EOK;
        }
        else
        {
            LOG_E("No memory to save '%p' handler", handler);
            err = -FM_ENOMEM;
        }
    }

    return err;
}

fm_err_t fm_pic_do_traps(void)
{
    fm_err_t err = -FM_ERROR;
    struct irq_traps *traps;

    fm_list_for_each_entry(traps, &_traps_nodes, list)
    {
        if (traps->handler(traps->data))
        {
            err = FM_EOK;

            break;
        }
    }

    return err;
}

fm_err_t fm_pic_handle_isr(struct fm_pic_irq *pirq)
{
    fm_err_t err = -FM_EEMPTY;
    fm_list_t *handler_nodes;
    struct fm_irq_desc *action;

    FM_ASSERT(pirq != FM_NULL);
    FM_ASSERT(pirq->pic != FM_NULL);

    handler_nodes = &pirq->isr.list;
    action = &pirq->isr.action;

    if (!fm_list_isempty(&pirq->children_nodes))
    {
        struct fm_pic_irq *child;

        fm_list_for_each_entry(child, &pirq->children_nodes, list)
        {
            fm_pic_irq_ack(child->irq);

            err = fm_pic_handle_isr(child);

            fm_pic_irq_eoi(child->irq);
        }
    }

    if (action->handler)
    {
        action->handler(pirq->irq, action->param);
    #ifdef FM_USING_INTERRUPT_INFO
        action->counter++;
    #endif

        if (!fm_list_isempty(handler_nodes))
        {
            struct fm_pic_isr *isr;

            fm_list_for_each_entry(isr, handler_nodes, list)
            {
                action = &isr->action;

                FM_ASSERT(action->handler != FM_NULL);

                action->handler(pirq->irq, action->param);
            #ifdef FM_USING_INTERRUPT_INFO
                action->counter++;
            #endif
            }
        }

        err = FM_EOK;
    }

    return err;
}

fm_weak fm_err_t fm_pic_user_extends(struct fm_pic *pic)
{
    return -FM_ENOSYS;
}


fm_err_t fm_pic_irq_init(void)
{
    fm_err_t err = FM_EOK;
    struct fm_pic *pic;

    fm_list_for_each_entry(pic, &_pic_nodes, list)
    {
        if (pic->ops->irq_init)
        {
            err = pic->ops->irq_init(pic);

            if (err)
            {
                LOG_E("PIC = %s init fail", pic->ops->name);
                break;
            }
        }
    }

    return err;
}

fm_err_t fm_pic_irq_finit(void)
{
    fm_err_t err = FM_EOK;
    struct fm_pic *pic;

    fm_list_for_each_entry(pic, &_pic_nodes, list)
    {
        if (pic->ops->irq_finit)
        {
            err = pic->ops->irq_finit(pic);

            if (err)
            {
                LOG_E("PIC = %s finit fail", pic->ops->name);
                break;
            }
        }
    }

    return err;
}

void fm_pic_irq_enable(int irq)
{
    struct fm_pic_irq *pirq = irq2pirq(irq);

    FM_ASSERT(pirq != FM_NULL);

    if (pirq->pic->ops->irq_enable)
    {
        pirq->pic->ops->irq_enable(pirq);
    }
}

void fm_pic_irq_disable(int irq)
{
    struct fm_pic_irq *pirq = irq2pirq(irq);

    FM_ASSERT(pirq != FM_NULL);

    if (pirq->pic->ops->irq_disable)
    {
        pirq->pic->ops->irq_disable(pirq);
    }
}

void fm_pic_irq_ack(int irq)
{
    struct fm_pic_irq *pirq = irq2pirq(irq);

    FM_ASSERT(pirq != FM_NULL);

    if (pirq->pic->ops->irq_ack)
    {
        pirq->pic->ops->irq_ack(pirq);
    }
}

void fm_pic_irq_mask(int irq)
{
    struct fm_pic_irq *pirq = irq2pirq(irq);

    FM_ASSERT(pirq != FM_NULL);

    if (pirq->pic->ops->irq_mask)
    {
        pirq->pic->ops->irq_mask(pirq);
    }
}

void fm_pic_irq_unmask(int irq)
{
    struct fm_pic_irq *pirq = irq2pirq(irq);

    FM_ASSERT(pirq != FM_NULL);

    if (pirq->pic->ops->irq_unmask)
    {
        pirq->pic->ops->irq_unmask(pirq);
    }
}

void fm_pic_irq_eoi(int irq)
{
    struct fm_pic_irq *pirq = irq2pirq(irq);

    FM_ASSERT(pirq != FM_NULL);

    if (pirq->pic->ops->irq_eoi)
    {
        pirq->pic->ops->irq_eoi(pirq);
    }
}

fm_err_t fm_pic_irq_set_priority(int irq, fm_uint32_t priority)
{
    fm_err_t err = -FM_EINVAL;
    struct fm_pic_irq *pirq = irq2pirq(irq);

    if (pirq)
    {
        if (pirq->pic->ops->irq_set_priority)
        {
            err = pirq->pic->ops->irq_set_priority(pirq, priority);

            if (!err)
            {
                pirq->priority = priority;
            }
        }
        else
        {
            err = -FM_ENOSYS;
        }
    }

    return err;
}

fm_uint32_t fm_pic_irq_get_priority(int irq)
{
    fm_uint32_t priority = FM_UINT32_MAX;
    struct fm_pic_irq *pirq = irq2pirq(irq);

    if (pirq)
    {
        priority = pirq->priority;
    }

    return priority;
}

fm_err_t fm_pic_irq_set_triger_mode(int irq, fm_uint32_t mode)
{
    fm_err_t err = -FM_EINVAL;
    struct fm_pic_irq *pirq;

    if ((~mode & FM_IRQ_MODE_MASK) && (pirq = irq2pirq(irq)))
    {
        if (pirq->pic->ops->irq_set_triger_mode)
        {
            err = pirq->pic->ops->irq_set_triger_mode(pirq, mode);

            if (!err)
            {
                pirq->mode = mode;
            }
        }
        else
        {
            err = -FM_ENOSYS;
        }
    }

    return err;
}

fm_uint32_t fm_pic_irq_get_triger_mode(int irq)
{
    fm_uint32_t mode = FM_UINT32_MAX;
    struct fm_pic_irq *pirq = irq2pirq(irq);

    if (pirq)
    {
        mode = pirq->mode;
    }

    return mode;
}

void fm_pic_irq_parent_enable(struct fm_pic_irq *pirq)
{
    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_enable)
    {
        pirq->pic->ops->irq_enable(pirq);
    }
}

void fm_pic_irq_parent_disable(struct fm_pic_irq *pirq)
{
    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_disable)
    {
        pirq->pic->ops->irq_disable(pirq);
    }
}

void fm_pic_irq_parent_ack(struct fm_pic_irq *pirq)
{
    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_ack)
    {
        pirq->pic->ops->irq_ack(pirq);
    }
}

void fm_pic_irq_parent_mask(struct fm_pic_irq *pirq)
{
    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_mask)
    {
        pirq->pic->ops->irq_mask(pirq);
    }
}

void fm_pic_irq_parent_unmask(struct fm_pic_irq *pirq)
{
    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_unmask)
    {
        pirq->pic->ops->irq_unmask(pirq);
    }
}

void fm_pic_irq_parent_eoi(struct fm_pic_irq *pirq)
{
    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_eoi)
    {
        pirq->pic->ops->irq_eoi(pirq);
    }
}

fm_err_t fm_pic_irq_parent_set_priority(struct fm_pic_irq *pirq, fm_uint32_t priority)
{
    fm_err_t err = -FM_ENOSYS;

    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_set_priority)
    {
        if (!(err = pirq->pic->ops->irq_set_priority(pirq, priority)))
        {
            pirq->priority = priority;
        }
    }

    return err;
}

fm_err_t fm_pic_irq_parent_set_triger_mode(struct fm_pic_irq *pirq, fm_uint32_t mode)
{
    fm_err_t err = -FM_ENOSYS;

    FM_ASSERT(pirq != FM_NULL);
    pirq = pirq->parent;

    if (pirq->pic->ops->irq_set_triger_mode)
    {
        if (!(err = pirq->pic->ops->irq_set_triger_mode(pirq, mode)))
        {
            pirq->mode = mode;
        }
    }

    return err;
}

fm_err_t fm_pic_init(void)
{
    return FM_EOK;
}

#include <nr_micro_shell.h>

#if defined(FM_USING_CONSOLE) && defined(NR_MICRO_SHELL_ENABLE)
static void list_irq(char argc, char*argv)
{
    fm_ubase_t level;
    fm_size_t irq_nr = 0;
    fm_bool_t dump_all = FM_FALSE;
    const char *const irq_modes[] =
    {
        [FM_IRQ_MODE_NONE] = "None",
        [FM_IRQ_MODE_EDGE_RISING] = "Edge-Rising",
        [FM_IRQ_MODE_EDGE_FALLING] = "Edge-Falling",
        [FM_IRQ_MODE_EDGE_BOTH] = "Edge-Both",
        [FM_IRQ_MODE_LEVEL_HIGH] = "Level-High",
        [FM_IRQ_MODE_LEVEL_LOW] = "Level-Low",
    };
    static char info[FM_CONSOLEBUF_SIZE];

    if (argc > 1)
    {
        if (!fm_strcmp("all", &argv[argv[1]]))
        {
            dump_all = FM_TRUE;
        }
    }

    fm_kprintf("%-*s %-*s %-*s %-*s %-*sUsers%s\n",
            10, "IRQ",
            10, "HW-IRQ",
            _pic_name_max, "PIC",
            12, "Mode",
        #ifdef FM_USING_INTERRUPT_INFO
            11, "Count",
            ""
        #else
            0, 0,
            "-Number"
        #endif
            );

    for (int i = 0; i < FM_ARRAY_SIZE(_pirq_hash); ++i)
    {
        struct fm_pic_irq *pirq = &_pirq_hash[i];

        if (!pirq->pic || !(dump_all || pirq->isr.action.handler))
        {
            continue;
        }

        snprintf(info, sizeof(info), "%-10d %-10d %-*s %-*s ",
                pirq->irq,
                pirq->hwirq,
                _pic_name_max, pirq->pic->ops->name,
                12, irq_modes[pirq->mode]);

        fm_kputs(info);

    #ifdef FM_USING_INTERRUPT_INFO
        fm_kprintf("%-10d ", pirq->isr.action.counter);
        fm_kputs(pirq->isr.action.name);
        fm_kputs("\n");

        if (!fm_list_isempty(&pirq->isr.list))
        {
            struct fm_pic_isr *repeat_isr;

            fm_list_for_each_entry(repeat_isr, &pirq->isr.list, list)
            {
                fm_kputs(info);
                fm_kprintf("%-10d ", repeat_isr->action.counter);
                fm_kputs(repeat_isr->action.name);
                fm_kputs("\n");
            }
        }
    #else
        fm_kprintf(" %d\n", fm_list_len(&pirq->isr.list));
    #endif

        ++irq_nr;
    }

    fm_kprintf("%d IRQs found\n", irq_nr);
}
NR_SHELL_CMD_EXPORT(list_irq, list_irq);
#endif /* FM_USING_CONSOLE && NR_MICRO_SHELL_ENABLE */
