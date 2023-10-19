#include <qk_state.h>
#include <qk_section.h>
#include <qk_type.h>

static state_info_t *state_info = NULL;

QK_SECTION_DEF_FLASH(qk_states, state_info_t *);

static void qk_state_polling()
{
    if (state_info == NULL)
    {
        return;
    }
    SAFE_INVOKE(state_info->handler);
}

POLLING_EXPORT(qk_state_polling);

int qk_state_transfer(uint32_t state)
{
    if (state_info != NULL)
    {
        if (state_info->state == state)
        {
            return 0;
        }
    }
    QK_SECTION_FOR_EACH(qk_states, state_info_t *, pptmp)
    {
        if (pptmp == NULL)
        {
            continue;
        }
        state_info_t *ptmp = *pptmp;
        if (ptmp == NULL)
        {
            continue;
        }
        if (ptmp->state == state)
        {
            if (state_info != NULL)
            {
                SAFE_INVOKE(state_info->exit);
                SAFE_INVOKE(ptmp->enter, state_info->state);
            }
            else
            {
                SAFE_INVOKE(ptmp->enter, ptmp->state);
            }
            state_info = ptmp;
            return 0;
        }
    }
    return -1;
}

int qk_state_invoke_event(uint32_t event, void *arg)
{
    int i = 0;
    if (state_info == NULL)
    {
        return -1;
    }
    if (state_info->event_table.p_event_table == NULL || state_info->event_table.number == 0)
    {
        return -2;
    }
    for (i = 0; i < state_info->event_table.number; i++)
    {
        if (state_info->event_table.p_event_table[i].event == event)
        {
            SAFE_INVOKE(state_info->event_table.p_event_table[i].handler, event, arg);
            break;
        }
    }
    return 0;
}

int qk_get_current_state()
{
    if (state_info == NULL)
    {
        return -1;
    }
    return state_info->state;
}
