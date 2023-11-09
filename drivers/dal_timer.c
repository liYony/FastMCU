#include <dal_timer.h>

dal_weak int mcu_timer_init(dal_timer_number_t timer, dal_timer_cntmode_t cntm, uint32_t period_max)
{
    return -1;
}

dal_weak int mcu_timer_start(dal_timer_number_t timer, dal_timer_mode_t mode, uint32_t period)
{
    return -1;
}

dal_weak int mcu_timer_stop(dal_timer_number_t timer)
{
    return -1;
}

dal_weak uint32_t mcu_timer_counter_get(dal_timer_number_t timer)
{
    return 0;
}

int dal_timer_init(dal_timer_number_t timer, dal_timer_cntmode_t cntm, uint32_t period_max)
{
    return mcu_timer_init(timer, cntm, period_max);
}

int dal_timer_start(dal_timer_number_t timer, dal_timer_mode_t mode, uint32_t period)
{
    return mcu_timer_start(timer, mode, period);
}

int dal_timer_stop(dal_timer_number_t timer)
{
    return mcu_timer_stop(timer);
}

uint32_t dal_timer_counter_get(dal_timer_number_t timer)
{
    return mcu_timer_counter_get(timer);
}

void dal_timer_attach_irq(dal_timer_up_attr_t *attr, dal_it_handler_t hdr)
{
    if (attr == NULL)
    {
        return;
    }

    attr->it.handler = hdr;
    attr->it.index = attr->timer;
    attr->it.it = DAL_HAL_IT_TIMER_FLOW;
    attr->it.user_data = attr;

    dal_it_register(&attr->it);
}
