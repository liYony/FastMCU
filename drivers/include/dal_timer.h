#ifndef __DAL_TIMER_H__
#define __DAL_TIMER_H__

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


#endif
