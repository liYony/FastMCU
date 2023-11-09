#ifndef __DAL_TIMER_H__
#define __DAL_TIMER_H__

#include <dal_type.h>
#include <dal_it.h>

typedef enum
{
    DAL_TIMER_0,
    DAL_TIMER_1,
    DAL_TIMER_2,
    DAL_TIMER_3,
    DAL_TIMER_4,
    DAL_TIMER_5,
    DAL_TIMER_6,
    DAL_TIMER_7,
    DAL_TIMER_8,
    DAL_TIMER_9,
    DAL_TIMER_10,
    DAL_TIMER_11,
    DAL_TIMER_12,
    DAL_TIMER_13,
    DAL_TIMER_14,
    DAL_TIMER_15,
    DAL_TIMER_16,
    DAL_TIMER_NUMBER
} dal_timer_number_t;

typedef enum
{
    DAL_TIMER_MODE_ONESHOT,
    DAL_TIMER_MODE_PERIOD
} dal_timer_mode_t;

typedef enum
{
    DAL_TIMER_CNTMODE_UP,
    DAL_TIMER_CNTMODE_DOWN
} dal_timer_cntmode_t;

typedef struct
{
    dal_timer_number_t timer;
    dal_it_t it;
    void *user_data;
} dal_timer_up_attr_t;

#define DAL_TIMER_CREATE_ATTR(name, tim, ud)        \
    static dal_timer_up_attr_t name = {             \
        .timer      = tim,                          \
        .user_data  = ud,                           \
    }

int mcu_timer_init(dal_timer_number_t timer, dal_timer_cntmode_t cntm, uint32_t period_max);
int mcu_timer_start(dal_timer_number_t timer, dal_timer_mode_t mode, uint32_t period);
int mcu_timer_stop(dal_timer_number_t timer);
uint32_t mcu_timer_counter_get(dal_timer_number_t timer);
int dal_timer_init(dal_timer_number_t timer, dal_timer_cntmode_t cntm, uint32_t period_max);
int dal_timer_start(dal_timer_number_t timer, dal_timer_mode_t mode, uint32_t period);
int dal_timer_stop(dal_timer_number_t timer);
uint32_t dal_timer_counter_get(dal_timer_number_t timer);
void dal_timer_attach_irq(dal_timer_up_attr_t *attr, dal_it_handler_t hdr);

#endif
