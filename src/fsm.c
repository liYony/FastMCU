/*
 * Copyright (c) 2023, liYony
 * Copyright (c) 2023, BabyOS
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-29     liYony       add Finite state machine(Learn from BabyOS)
 */

#include <fmcu.h>

static fm_slist_t _fsm_list = FM_SLIST_OBJECT_INIT(_fsm_list);

fsm_info_t fm_fsm_find(const char *name)
{
    struct fm_slist_node *node = FM_NULL;
    struct fsm_info *fsm_info = FM_NULL;

    if (name == FM_NULL)
        return FM_NULL;

    fm_slist_for_each(node, &_fsm_list)
    {
        fsm_info = fm_slist_entry(node, struct fsm_info, list);
        if (fm_strncmp(fsm_info->name, name, FM_NAME_MAX) == 0)
        {
            return fsm_info;
        }
    }

    return FM_NULL;
}

static s_info_t fm_fsm_state_find(fsm_info_t fsm, fm_uint32_t state)
{
    struct fm_slist_node *node = FM_NULL;
    struct s_info *s_info = FM_NULL;

    fm_slist_for_each(node, &fsm->s_list)
    {
        s_info = fm_slist_entry(node, struct s_info, list);
        if (s_info->state == state)
        {
            return s_info;
        }
    }

    return FM_NULL;
}

static s_event_t fm_fsm_event_find(s_info_t s, fm_uint32_t event)
{
    struct fm_slist_node *node = FM_NULL;
    struct s_event *s_event = FM_NULL;

    fm_slist_for_each(node, &s->e_list)
    {
        s_event = fm_slist_entry(node, struct s_event, list);
        if (s_event->event == event)
        {
            return s_event;
        }
    }

    return FM_NULL;
}

fm_err_t fm_fsm_register(fsm_info_t fsm, const char *name)
{
    if (fsm == FM_NULL)
        return -FM_ERROR;

    if (fm_fsm_find(name) != FM_NULL)
        return -FM_ERROR;

    fsm->info = FM_NULL;

    fm_strncpy(fsm->name, name, FM_NAME_MAX); /* copy name */
    fm_slist_append(&_fsm_list, &fsm->list);
    fm_slist_init(&fsm->s_list);
    
    return FM_EOK;
}

fm_err_t fm_fsm_unregister(fsm_info_t fsm)
{
    /* parameter check */
    FM_ASSERT(fsm != FM_NULL);

    fm_slist_remove(&_fsm_list, &(fsm->list));

    return FM_EOK;
}

fm_err_t fm_fsm_state_bind(fsm_info_t fsm,
                           fm_uint32_t state,
                           s_enter_fn_t fn_enter,
                           s_exit_fn_t fn_exit,
                           s_exec_fn_t fn_exec)
{
    struct s_info *s = (struct s_info *)fm_malloc(sizeof(struct s_info));

    if (fsm == FM_NULL || s == FM_NULL)
        return -FM_ERROR;

    if (fm_fsm_state_find(fsm, state) != FM_NULL)
        return -FM_ERROR;

    s->state = state;
    s->fn_enter = fn_enter;
    s->fn_exit = fn_exit;
    s->fn_exec = fn_exec;

    fm_slist_append(&fsm->s_list, &s->list);
    fm_slist_init(&s->e_list);
    
    return FM_EOK;
}

fm_err_t fm_fsm_state_unbind(fsm_info_t fsm,
                             fm_uint32_t state)
{
    if (fsm == FM_NULL)
        return -FM_ERROR;

    struct s_info *s = fm_fsm_state_find(fsm, state);
    if (s == FM_NULL)
        return -FM_ERROR;

    fm_slist_remove(&fsm->s_list, &(s->list));

    /* free */
    struct fm_slist_node *node = FM_NULL;
    struct s_event *s_event = FM_NULL;

    fm_slist_for_each(node, &s->e_list)
    {
        s_event = fm_slist_entry(node, struct s_event, list);

        fm_free(s_event);
    }
    fm_free(s);

    return FM_EOK;
}
fm_err_t fm_fsm_event_bind(fsm_info_t fsm,
                           fm_uint32_t state,
                           fm_uint32_t event,
                           s_event_fn_t fn_event)
{
    struct s_info *s = fm_fsm_state_find(fsm, state);
    struct s_event *e = (struct s_event *)fm_malloc(sizeof(struct s_event));

    if (s == FM_NULL || e == FM_NULL)
        return -FM_ERROR;

    if (fm_fsm_event_find(s, event) != FM_NULL)
        return -FM_ERROR;

    e->event = event;
    e->fn_event = fn_event;

    fm_slist_append(&s->e_list, &e->list);
    
    return FM_EOK;
}

fm_err_t fm_fsm_event_unbind(fsm_info_t fsm,
                             fm_uint32_t state,
                             fm_uint32_t event)
{
    if (fsm == FM_NULL)
        return -FM_ERROR;

    struct s_info *s = fm_fsm_state_find(fsm, state);
    if (s == FM_NULL)
        return -FM_ERROR;

    struct s_event *e = fm_fsm_event_find(s, event);
    if (e == FM_NULL)
        return -FM_ERROR;

    fm_slist_remove(&s->e_list, &(e->list));

    fm_free(e);

    return FM_EOK;
}

void fm_fsm_show(void)
{
    struct fm_slist_node *fsm_node = FM_NULL;
    struct fm_slist_node *s_node = FM_NULL;
    struct fm_slist_node *e_node = FM_NULL;

    struct s_event *s_event = FM_NULL;
    struct s_info *s_info = FM_NULL;
    struct fsm_info *fsm_info = FM_NULL;
    fm_slist_for_each(fsm_node, &_fsm_list)
    {
        fsm_info = fm_slist_entry(fsm_node, struct fsm_info, list);
        fm_slist_for_each(s_node, &fsm_info->s_list)
        {
            s_info = fm_slist_entry(s_node, struct s_info, list);
            fm_slist_for_each(e_node, &s_info->e_list)
            {
                s_event = fm_slist_entry(e_node, struct s_event, list);
                fm_kprintf("%s %d %d\r\n", fsm_info->name, s_info->state, s_event->event);
            }
        }
    }
}

fm_err_t fm_fsm_transfer(fsm_info_t fsm, fm_uint32_t state)
{
    if (fsm == FM_NULL)
        return -FM_ERROR;

    struct s_info *s = fm_fsm_state_find(fsm, state);
    if (s == FM_NULL)
        return -FM_ERROR;

    if (fsm->info == FM_NULL)
    {
        FM_SAFE_INVOKE(s->fn_enter, s->state);
    }
    else if (fsm->info->state == state)
    {
        ;
    }
    else
    {
        FM_SAFE_INVOKE(fsm->info->fn_exit);
        FM_SAFE_INVOKE(s->fn_enter, fsm->info->state);
    }

    fsm->info = s;

    return FM_EOK;
}

fm_err_t fm_fsm_invoke_event(fsm_info_t fsm, fm_uint32_t event, void *arg)
{
    if (fsm == FM_NULL)
        return -FM_ERROR;

    if (fsm->info == FM_NULL)
        return -FM_ERROR;

    struct fm_slist_node *node = FM_NULL;
    struct s_event *s_event = FM_NULL;

    fm_slist_for_each(node, &fsm->info->e_list)
    {
        s_event = fm_slist_entry(node, struct s_event, list);
        if (s_event->event == event)
        {
            FM_SAFE_INVOKE(s_event->fn_event, event, arg);
            break;
        }
    }

    return FM_EOK;
}

fm_uint32_t fm_fsm_current_state(fsm_info_t fsm)
{
    if (fsm == FM_NULL)
        return FM_STATE_NONE;

    if (fsm->info == FM_NULL)
        return FM_STATE_NONE;

    return fsm->info->state;
}

void fm_fsm_execute(void)
{
    struct fm_slist_node *node = FM_NULL;
    struct fsm_info *fsm_info = FM_NULL;

    fm_slist_for_each(node, &_fsm_list)
    {
        fsm_info = fm_slist_entry(node, struct fsm_info, list);
        if (fsm_info->info != FM_NULL)
        {
            FM_SAFE_INVOKE(fsm_info->info->fn_exec);
        }
    }
}
