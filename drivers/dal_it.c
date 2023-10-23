#include "dal.h"

static LIST_HEAD(dal_it_list_head);

dal_weak void mcu_int_enable()
{
    ;
}

dal_weak void mcu_int_disable()
{
    ;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void dal_int_enable()
{
    mcu_int_enable();
}

void dal_int_disable()
{
    mcu_int_disable();
}

int dal_it_register(dal_it_t *pit)
{
    if (pit == NULL)
    {
        return -1;
    }
    if (pit->handler == NULL)
    {
        return -1;
    }
    list_add(&pit->head, &dal_it_list_head);
    return 0;
}

int dal_it_invoke(dal_it_number_t it, uint8_t index, dal_it_param_t *param)
{
    dal_it_t         *pdalit = NULL;
    struct list_head *pos;
    list_for_each(pos, &dal_it_list_head)
    {
        pdalit = list_entry(pos, dal_it_t, head);
        if (it == pdalit->it && index == pdalit->index)
        {
            if (pdalit->handler)
            {
                pdalit->handler(it, index, param, pdalit->user_data);
                return 0;
            }
        }
    }
    return -1;
}
