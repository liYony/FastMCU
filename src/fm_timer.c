#include <fm_timer.h>
#include <dal_system.h>

#if defined(TIMER_ENABLE)

#if defined(TIMER_RUN_SECTION)
#include <fm_section.h>
#endif /* TIMER_RUN_SECTION */

static LIST_HEAD(fm_timer_list_head);

static fm_timer_attr_t *fm_timer_find(fm_timer_id_t id)
{
    struct list_head *pos   = NULL;
    fm_timer_attr_t     *pattr = NULL;
    list_for_each(pos, &fm_timer_list_head)
    {
        pattr = list_entry(pos, fm_timer_attr_t, list);
        if (pattr == ((fm_timer_attr_t *)id))
        {
            break;
        }
        pattr = NULL;
    }
    return pattr;
}

#if defined(TIMER_RUN_SECTION)
static void fm_timer_core(void)
#else
void fm_timer_core(void)
#endif /* TIMER_RUN_SECTION */
{
    struct list_head *pos   = NULL;
    fm_timer_attr_t     *pattr = NULL;
    list_for_each(pos, &fm_timer_list_head)
    {
        pattr = list_entry(pos, fm_timer_attr_t, list);
        if (pattr != NULL && pattr->func != NULL && pattr->enable == 1)
        {
            if (dal_get_systick_plus() >= pattr->tick)
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

#if defined(TIMER_RUN_SECTION)
POLLING_EXPORT(fm_timer_core);
#endif /* TIMER_RUN_SECTION */

fm_timer_id_t fm_timer_create(fm_timer_func_t func, fm_timer_type_t type, void *argument, fm_timer_attr_t *attr)
{
    if (attr == NULL || func == NULL)
    {
        return NULL;
    }
    if (fm_timer_find(attr) != NULL)
    {
        return attr;
    }
    attr->func   = func;
    attr->arg    = argument;
    attr->type   = type;
    attr->enable = 0;
    list_add(&attr->list, &fm_timer_list_head);
    return attr;
}

int fm_timer_delete(fm_timer_id_t id)
{
    fm_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return -1;
    }
    pattr = fm_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    __list_del(pattr->list.prev, pattr->list.next);
    return 0;
}

int fm_timer_start(fm_timer_id_t id, uint32_t ms)
{
    fm_timer_attr_t *pattr = NULL;
    if (id == NULL || ms == 0)
    {
        return -1;
    }
    pattr = fm_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    pattr->cycle  = ms;
    pattr->tick   = dal_get_systick_plus() + MS2TICKS(ms);
    pattr->enable = 1;
    return 0;
}

int fm_timer_stop(fm_timer_id_t id)
{
    fm_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return -1;
    }
    pattr = fm_timer_find(id);
    if (pattr == NULL)
    {
        return -1;
    }
    pattr->enable = 0;
    return 0;
}

int fm_timer_is_running(fm_timer_id_t id)
{
    fm_timer_attr_t *pattr = NULL;
    if (id == NULL)
    {
        return 0;
    }
    pattr = fm_timer_find(id);
    if (pattr == NULL)
    {
        return 0;
    }
    return (pattr->enable == 1);
}

#endif /* TIMER_ENABLE */
