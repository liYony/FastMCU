#include <qk_timer.h>
#include <qk_halport.h>
#include <qk_section.h>
#include <qk_memh.h>

#if defined(TIMER_ENABLE)

static LIST_HEAD(q_timer_list_head);

static qk_timer_attr_t *qk_timer_find(qk_timer_id_t id)
{
    struct list_head *pos   = NULL;
    qk_timer_attr_t     *pattr = NULL;
    list_for_each(pos, &q_timer_list_head)
    {
        pattr = list_entry(pos, qk_timer_attr_t, list);
        if (pattr == ((qk_timer_attr_t *)id))
        {
            break;
        }
        pattr = NULL;
    }
    return pattr;
}

qk_timer_id_t qk_timer_init(qk_timer_func_t func, qk_timer_type_t type, void *argument, qk_timer_attr_t *attr)
{
    if (attr == NULL || func == NULL)
    {
        return NULL;
    }
    if (qk_timer_find(attr) != NULL)
    {
        return attr;
    }
    attr->func   = func;
    attr->arg    = argument;
    attr->type   = type;
    attr->enable = 0;
    list_add(&attr->list, &q_timer_list_head);
    return attr;
}

int qk_timer_delete(qk_timer_id_t id)
{
    qk_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return -1;
    }
    pattr = qk_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    __list_del(pattr->list.prev, pattr->list.next);
    return 0;
}

qk_timer_id_t qk_timer_create(qk_timer_func_t func, qk_timer_type_t type, void *argument)
{
    qk_timer_attr_t *attr = (qk_timer_attr_t *)qk_malloc(sizeof(qk_timer_attr_t));
    
    if (attr == NULL || func == NULL)
    {
        return NULL;
    }
    if (qk_timer_find(attr) != NULL)
    {
        return attr;
    }
    attr->func   = func;
    attr->arg    = argument;
    attr->type   = type;
    attr->enable = 0;
    list_add(&attr->list, &q_timer_list_head);
    return attr;
}

int qk_timer_detach(qk_timer_id_t id)
{
    qk_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return -1;
    }
    pattr = qk_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    __list_del(pattr->list.prev, pattr->list.next);
    qk_free((void *)id);
    
    return 0;
}

int qk_timer_start(qk_timer_id_t id, uint32_t ms)
{
    qk_timer_attr_t *pattr = NULL;
    if (id == NULL || ms == 0)
    {
        return -1;
    }
    pattr = qk_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    pattr->cycle  = ms;
    pattr->tick   = qk_hal_get_systick_plus() + MS2TICKS(ms);
    pattr->enable = 1;
    return 0;
}

int qk_timer_stop(qk_timer_id_t id)
{
    qk_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return -1;
    }
    pattr = qk_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    pattr->enable = 0;
    return 0;
}

uint32_t qk_timer_is_running(qk_timer_id_t id)
{
    qk_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return 0;
    }
    pattr = qk_timer_find(id);
    if (pattr == NULL)
    {
        return 0;
    }
    return (pattr->enable == 1);
}

void qk_timer_core(void)
{
    struct list_head *pos   = NULL;
    qk_timer_attr_t     *pattr = NULL;
    list_for_each(pos, &q_timer_list_head)
    {
        pattr = list_entry(pos, qk_timer_attr_t, list);
        if (pattr != NULL && pattr->func != NULL && pattr->enable == 1)
        {
            if (qk_hal_get_systick_plus() >= pattr->tick)
            {
                pattr->func(pattr->arg);
                if (pattr->type == TIMER_ONCE)
                {
                    pattr->enable = 0;
                }
                else
                {
                    pattr->tick += MS2TICKS(pattr->cycle);
                }
            }
        }
    }
}

POLLING_EXPORT(qk_timer_core);

#endif /* TIMER_ENABLE */
