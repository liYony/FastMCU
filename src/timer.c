/*
 * Copyright (c) 2023-2024, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-01-03     liYony       first version
 */

#include <fmdef.h>
#include <fmcu.h>

fm_timer_t fm_timer_find(const char *name)
{
    return (fm_timer_t)fm_object_find(name, FM_Object_Class_Timer);
}

fm_err_t fm_timer_init(fm_timer_t timer,
                       const char *name,
                       void (*timeout)(void *parameter),
                       void *parameter,
                       fm_uint8_t type)
{
    /* parameter check */
    FM_ASSERT(name != FM_NULL);
    FM_ASSERT(timer != FM_NULL);
    FM_ASSERT(timeout != FM_NULL);

    if (fm_timer_find(name) != FM_NULL)
        return -FM_ERROR;
   
    /* timer object initialization */
    fm_object_init(&(timer->parent), FM_Object_Class_Timer, name);

    timer->timeout_func = timeout;
    timer->parameter = parameter;
    timer->type = type;
    timer->enable = FM_FALSE;

    return FM_EOK;
}

fm_err_t fm_timer_detach(fm_timer_t timer)
{
    /* parameter check */
    FM_ASSERT(timer != FM_NULL);
    FM_ASSERT(fm_object_get_type(&timer->parent) == FM_Object_Class_Timer);
    FM_ASSERT(fm_object_is_systemobject(&timer->parent));

    timer->timeout_func = FM_NULL;
    timer->parameter = FM_NULL;
    timer->enable = FM_FALSE;

    fm_object_detach(&(timer->parent));

    return FM_EOK;
}

#ifdef FM_USING_HEAP
fm_timer_t fm_timer_create(const char *name,
                           void (*timeout)(void *parameter),
                           void *parameter,
                           fm_uint8_t type)
{
    /* parameter check */
    FM_ASSERT(name != FM_NULL);
    FM_ASSERT(timeout != FM_NULL);

    if (fm_timer_find(name) != FM_NULL)
        return FM_NULL;

    struct fm_timer *timer = (struct fm_timer *)fm_object_allocate(FM_Object_Class_Timer, name);
    if (timer == FM_NULL)
    {
        return FM_NULL;
    }

    timer->timeout_func = timeout;
    timer->parameter = parameter;
    timer->type = type;
    timer->enable = FM_FALSE;

    return timer;
}

fm_err_t fm_timer_delete(fm_timer_t timer)
{
    /* parameter check */
    FM_ASSERT(timer != FM_NULL);
    FM_ASSERT(fm_object_get_type(&timer->parent) == FM_Object_Class_Timer);
    FM_ASSERT(fm_object_is_systemobject(&timer->parent) == FM_FALSE);

    fm_object_delete(&(timer->parent));

    return FM_EOK;
}
#endif /* FM_USING_HEAP */

static int fm_timer_loop(void)
{
    struct fm_list_node *node = FM_NULL;
    struct fm_timer *fm_timer = FM_NULL;
    
    struct fm_object_information *info;
    info = fm_object_get_information((enum fm_object_class_type)FM_Object_Class_Timer);
    fm_list_for_each(node, &info->object_list)
    {
        fm_timer = (struct fm_timer *)fm_slist_entry(node, struct fm_object, list);
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

    return 0;
}

INIT_APP_EXPORT(fm_timer_loop);

fm_err_t fm_timer_start(fm_timer_t timer, fm_uint32_t ms)
{
    /* parameter check */
    FM_ASSERT(timer != FM_NULL);
    FM_ASSERT(fm_object_get_type(&timer->parent) == FM_Object_Class_Timer);

    timer->cycle = ms;
    timer->tick = fm_get_systick();
    timer->enable = FM_TRUE;

    return FM_EOK;
}

fm_err_t fm_timer_stop(fm_timer_t timer)
{
    /* timer check */
    FM_ASSERT(timer != FM_NULL);
    FM_ASSERT(fm_object_get_type(&timer->parent) == FM_Object_Class_Timer);

    timer->enable = FM_FALSE;

    return FM_EOK;
}
