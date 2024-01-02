#include <fmdef.h>
#include <fmcu.h>

static fm_slist_t _timer_list = FM_SLIST_OBJECT_INIT(_timer_list);

fm_timer_t fm_timer_find(const char *name)
{
    struct fm_slist_node *node = FM_NULL;
    struct fm_timer *fm_timer = FM_NULL;

    if (name == FM_NULL)
        return FM_NULL;

    fm_slist_for_each(node, &_timer_list)
    {
        fm_timer = fm_slist_entry(node, struct fm_timer, list);
        if (fm_strncmp(fm_timer->name, name, FM_NAME_MAX) == 0)
        {
            return fm_timer;
        }
    }

    return FM_NULL;
}

static void _timer_init(fm_timer_t timer,
                        void (*timeout)(void *parameter),
                        void *parameter,
                        fm_uint8_t type)
{
    timer->timeout_func = timeout;
    timer->parameter = parameter;

    timer->type = type;
    timer->enable = FM_FALSE;

    fm_slist_append(&_timer_list, &timer->list);
}

fm_err_t fm_timer_init(fm_timer_t timer,
                       const char *name,
                       void (*timeout)(void *parameter),
                       void *parameter,
                       fm_uint8_t type)
{
    if (timer == FM_NULL || name == FM_NULL || timeout == FM_NULL)
        return -FM_ERROR;

    if (fm_timer_find(name) != FM_NULL)
        return -FM_ERROR;

    _timer_init(timer, timeout, parameter, type);

    return FM_EOK;
}

fm_err_t fm_timer_detach(fm_timer_t timer)
{
    if (timer == FM_NULL)
        return -FM_ERROR;

    timer->timeout_func = FM_NULL;
    timer->parameter = FM_NULL;
    timer->enable = FM_FALSE;

    fm_slist_remove(&_timer_list, &timer->list);

    return FM_EOK;
}

#ifdef FM_USING_HEAP
fm_timer_t fm_timer_create(const char *name,
                           void (*timeout)(void *parameter),
                           void *parameter,
                           fm_uint8_t type)
{
    if (name == FM_NULL || timeout == FM_NULL)
        return FM_NULL;

    if (fm_timer_find(name) != FM_NULL)
        return FM_NULL;

    struct fm_timer *timer = (struct fm_timer *)fm_malloc(sizeof(struct fm_timer));
    if (timer == FM_NULL)
        return FM_NULL;

    _timer_init(timer, timeout, parameter, type);

    return timer;
}

fm_err_t fm_timer_delete(fm_timer_t timer)
{
    if (timer == FM_NULL)
        return -FM_ERROR;

    fm_slist_remove(&_timer_list, &timer->list);
    fm_free(timer);

    return FM_EOK;
}
#endif /* FM_USING_HEAP */

void fm_timer_loop(void)
{
    struct fm_slist_node *node = FM_NULL;
    struct fm_timer *fm_timer = FM_NULL;
    fm_slist_for_each(node, &_timer_list)
    {
        fm_timer = fm_slist_entry(node, struct fm_timer, list);
        if (fm_timer != FM_NULL &&
            fm_timer->timeout_func != FM_NULL &&
            fm_timer->enable == 1)
        {
            if (fm_get_systick() - fm_timer->tick > FM_MS2TICKS(fm_timer->cycle))
            {
                fm_timer->timeout_func(fm_timer->parameter);
                if (fm_timer->type == FM_TIMER_FLAG_ONE_SHOT)
                {
                    fm_timer->enable = 0;
                }
                else
                {
                    fm_timer->tick = fm_get_systick();
                }
            }
        }
    }
}

fm_err_t fm_timer_start(fm_timer_t timer, fm_uint32_t ms)
{
    if (timer == FM_NULL)
        return -FM_ERROR;

    timer->cycle = ms;
    timer->tick = fm_get_systick();
    timer->enable = FM_TRUE;

    return FM_EOK;
}

fm_err_t fm_timer_stop(fm_timer_t timer)
{
    if (timer == FM_NULL)
        return -FM_ERROR;

    timer->enable = FM_FALSE;

    return FM_EOK;
}
