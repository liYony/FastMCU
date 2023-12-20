/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#ifndef __HWTIMER_H__
#define __HWTIMER_H__

#include <fmcu.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Timer Control Command */
typedef enum
{
    HWTIMER_CTRL_FREQ_SET = FM_DEVICE_CTRL_BASE(Timer) + 0x01,           /* set the count frequency */
    HWTIMER_CTRL_STOP = FM_DEVICE_CTRL_BASE(Timer) + 0x02,               /* stop timer */
    HWTIMER_CTRL_INFO_GET = FM_DEVICE_CTRL_BASE(Timer) + 0x03,           /* get a timer feature information */
    HWTIMER_CTRL_MODE_SET = FM_DEVICE_CTRL_BASE(Timer) + 0x04            /* Setting the timing mode(oneshot/period) */
} fm_hwtimer_ctrl_t;

/* Timing Mode */
typedef enum
{
    HWTIMER_MODE_ONESHOT = 0x01,
    HWTIMER_MODE_PERIOD
} fm_hwtimer_mode_t;

/* Time Value */
typedef struct fm_hwtimerval
{
    fm_int32_t sec;      /* second */
    fm_int32_t usec;     /* microsecond */
} fm_hwtimerval_t;

#define HWTIMER_CNTMODE_UP      0x01 /* increment count mode */
#define HWTIMER_CNTMODE_DW      0x02 /* decreasing count mode */

struct fm_hwtimer_device;

struct fm_hwtimer_ops
{
    void (*init)(struct fm_hwtimer_device *timer, fm_uint32_t state);
    fm_err_t (*start)(struct fm_hwtimer_device *timer, fm_uint32_t cnt, fm_hwtimer_mode_t mode);
    void (*stop)(struct fm_hwtimer_device *timer);
    fm_uint32_t (*count_get)(struct fm_hwtimer_device *timer);
    fm_err_t (*control)(struct fm_hwtimer_device *timer, fm_uint32_t cmd, void *args);
};

/* Timer Feature Information */
struct fm_hwtimer_info
{
    fm_int32_t maxfreq;    /* the maximum count frequency timer support */
    fm_int32_t minfreq;    /* the minimum count frequency timer support */
    fm_uint32_t maxcnt;    /* counter maximum value */
    fm_uint8_t  cntmode;   /* count mode (inc/dec) */
};

typedef struct fm_hwtimer_device
{
    struct fm_device parent;
    const struct fm_hwtimer_ops *ops;
    const struct fm_hwtimer_info *info;

    fm_int32_t freq;                /* counting frequency set by the user */
    fm_int32_t overflow;            /* timer overflows */
    float period_sec;
    fm_int32_t cycles;              /* how many times will generate a timeout event after overflow */
    fm_int32_t reload;              /* reload cycles(using in period mode) */
    fm_hwtimer_mode_t mode;         /* timing mode(oneshot/period) */
} fm_hwtimer_t;

fm_err_t fm_device_hwtimer_register(fm_hwtimer_t *timer, const char *name, void *user_data);
void fm_device_hwtimer_isr(fm_hwtimer_t *timer);

#ifdef __cplusplus
}
#endif

#endif
