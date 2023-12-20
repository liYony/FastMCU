/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2015-08-31     heyuanjie87    first version
 */

#include <fmdevice.h>

#define DBG_TAG "hwtimer"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

fm_inline fm_uint32_t timeout_calc(fm_hwtimer_t *timer, fm_hwtimerval_t *tv)
{
    float overflow;
    float timeout;
    fm_uint32_t counter;
    int i, index = 0;
    float tv_sec;
    float devi_min = 1;
    float devi;

    /* changed to second */
    overflow = timer->info->maxcnt / (float)timer->freq;
    tv_sec = tv->sec + tv->usec / (float)1000000;

    if (tv_sec < (1 / (float)timer->freq))
    {
        /* little timeout */
        i = 0;
        timeout = 1 / (float)timer->freq;
    }
    else
    {
        for (i = 1; i > 0; i++)
        {
            timeout = tv_sec / i;

            if (timeout <= overflow)
            {
                counter = (fm_uint32_t)(timeout * timer->freq);
                devi = tv_sec - (counter / (float)timer->freq) * i;
                /* Minimum calculation error */
                if (devi > devi_min)
                {
                    i = index;
                    timeout = tv_sec / i;
                    break;
                }
                else if (devi == 0)
                {
                    break;
                }
                else if (devi < devi_min)
                {
                    devi_min = devi;
                    index = i;
                }
            }
        }
    }

    timer->cycles = i;
    timer->reload = i;
    timer->period_sec = timeout;
    counter = (fm_uint32_t)(timeout * timer->freq);

    return counter;
}

static fm_err_t fm_hwtimer_init(struct fm_device *dev)
{
    fm_err_t result = FM_EOK;
    fm_hwtimer_t *timer;

    timer = (fm_hwtimer_t *)dev;
    /* try to change to 1MHz */
    if ((1000000 <= timer->info->maxfreq) && (1000000 >= timer->info->minfreq))
    {
        timer->freq = 1000000;
    }
    else
    {
        timer->freq = timer->info->minfreq;
    }
    timer->mode = HWTIMER_MODE_ONESHOT;
    timer->cycles = 0;
    timer->overflow = 0;

    if (timer->ops->init)
    {
        timer->ops->init(timer, 1);
    }
    else
    {
        result = -FM_ENOSYS;
    }

    return result;
}

static fm_err_t fm_hwtimer_open(struct fm_device *dev, fm_uint16_t oflag)
{
    fm_err_t result = FM_EOK;
    fm_hwtimer_t *timer;

    timer = (fm_hwtimer_t *)dev;
    if (timer->ops->control != FM_NULL)
    {
        timer->ops->control(timer, HWTIMER_CTRL_FREQ_SET, &timer->freq);
    }
    else
    {
        result = -FM_ENOSYS;
    }

    return result;
}

static fm_err_t fm_hwtimer_close(struct fm_device *dev)
{
    fm_err_t result = FM_EOK;
    fm_hwtimer_t *timer;

    timer = (fm_hwtimer_t *)dev;
    if (timer->ops->init != FM_NULL)
    {
        timer->ops->init(timer, 0);
    }
    else
    {
        result = -FM_ENOSYS;
    }

    dev->flag &= ~FM_DEVICE_FLAG_ACTIVATED;
    dev->rx_indicate = FM_NULL;

    return result;
}

static fm_ssize_t fm_hwtimer_read(struct fm_device *dev, fm_off_t pos, void *buffer, fm_size_t size)
{
    fm_hwtimer_t *timer;
    fm_hwtimerval_t tv;
    fm_uint32_t cnt;
    fm_base_t level;
    fm_int32_t overflow;
    float t;

    timer = (fm_hwtimer_t *)dev;
    if (timer->ops->count_get == FM_NULL)
        return 0;

    level = fm_hw_interrupt_disable();
    cnt = timer->ops->count_get(timer);
    overflow = timer->overflow;
    fm_hw_interrupt_enable(level);

    if (timer->info->cntmode == HWTIMER_CNTMODE_DW)
    {
        cnt = (fm_uint32_t)(timer->freq * timer->period_sec) - cnt;
    }

    t = overflow * timer->period_sec + cnt / (float)timer->freq;
    tv.sec = (fm_int32_t)t;
    tv.usec = (fm_int32_t)((t - tv.sec) * 1000000);
    size = size > sizeof(tv) ? sizeof(tv) : size;
    fm_memcpy(buffer, &tv, size);

    return size;
}

static fm_ssize_t fm_hwtimer_write(struct fm_device *dev, fm_off_t pos, const void *buffer, fm_size_t size)
{
    fm_base_t level;
    fm_uint32_t t;
    fm_hwtimer_mode_t opm = HWTIMER_MODE_PERIOD;
    fm_hwtimer_t *timer;

    timer = (fm_hwtimer_t *)dev;
    if ((timer->ops->start == FM_NULL) || (timer->ops->stop == FM_NULL))
        return 0;

    if (size != sizeof(fm_hwtimerval_t))
        return 0;

    timer->ops->stop(timer);

    level = fm_hw_interrupt_disable();
    timer->overflow = 0;
    fm_hw_interrupt_enable(level);

    t = timeout_calc(timer, (fm_hwtimerval_t *)buffer);
    if ((timer->cycles <= 1) && (timer->mode == HWTIMER_MODE_ONESHOT))
    {
        opm = HWTIMER_MODE_ONESHOT;
    }

    if (timer->ops->start(timer, t, opm) != FM_EOK)
        size = 0;

    return size;
}

static fm_err_t fm_hwtimer_control(struct fm_device *dev, int cmd, void *args)
{
    fm_base_t level;
    fm_err_t result = FM_EOK;
    fm_hwtimer_t *timer;

    timer = (fm_hwtimer_t *)dev;

    switch (cmd)
    {
    case HWTIMER_CTRL_STOP:
    {
        if (timer->ops->stop != FM_NULL)
        {
            timer->ops->stop(timer);
        }
        else
        {
            result = -FM_ENOSYS;
        }
    }
    break;
    case HWTIMER_CTRL_FREQ_SET:
    {
        fm_int32_t *f;

        if (args == FM_NULL)
        {
            result = -FM_EEMPTY;
            break;
        }

        f = (fm_int32_t *)args;
        if ((*f > timer->info->maxfreq) || (*f < timer->info->minfreq))
        {
            LOG_W("frequency setting out of range! It will maintain at %d Hz", timer->freq);
            result = -FM_EINVAL;
            break;
        }

        if (timer->ops->control != FM_NULL)
        {
            result = timer->ops->control(timer, cmd, args);
            if (result == FM_EOK)
            {
                level = fm_hw_interrupt_disable();
                timer->freq = *f;
                fm_hw_interrupt_enable(level);
            }
        }
        else
        {
            result = -FM_ENOSYS;
        }
    }
    break;
    case HWTIMER_CTRL_INFO_GET:
    {
        if (args == FM_NULL)
        {
            result = -FM_EEMPTY;
            break;
        }

        *((struct fm_hwtimer_info *)args) = *timer->info;
    }
    break;
    case HWTIMER_CTRL_MODE_SET:
    {
        fm_hwtimer_mode_t *m;

        if (args == FM_NULL)
        {
            result = -FM_EEMPTY;
            break;
        }

        m = (fm_hwtimer_mode_t *)args;

        if ((*m != HWTIMER_MODE_ONESHOT) && (*m != HWTIMER_MODE_PERIOD))
        {
            result = -FM_ERROR;
            break;
        }
        level = fm_hw_interrupt_disable();
        timer->mode = *m;
        fm_hw_interrupt_enable(level);
    }
    break;
    default:
    {
        result = -FM_ENOSYS;
    }
    break;
    }

    return result;
}

void fm_device_hwtimer_isr(fm_hwtimer_t *timer)
{
    fm_base_t level;

    FM_ASSERT(timer != FM_NULL);

    level = fm_hw_interrupt_disable();

    timer->overflow++;

    if (timer->cycles != 0)
    {
        timer->cycles--;
    }

    if (timer->cycles == 0)
    {
        timer->cycles = timer->reload;

        fm_hw_interrupt_enable(level);

        if (timer->mode == HWTIMER_MODE_ONESHOT)
        {
            if (timer->ops->stop != FM_NULL)
            {
                timer->ops->stop(timer);
            }
        }

        if (timer->parent.rx_indicate != FM_NULL)
        {
            timer->parent.rx_indicate(&timer->parent, sizeof(struct fm_hwtimerval));
        }
    }
    else
    {
        fm_hw_interrupt_enable(level);
    }
}

#ifdef FM_USING_DEVICE_OPS
const static struct fm_device_ops hwtimer_ops =
    {
        fm_hwtimer_init,
        fm_hwtimer_open,
        fm_hwtimer_close,
        fm_hwtimer_read,
        fm_hwtimer_write,
        fm_hwtimer_control};
#endif

fm_err_t fm_device_hwtimer_register(fm_hwtimer_t *timer, const char *name, void *user_data)
{
    struct fm_device *device;

    FM_ASSERT(timer != FM_NULL);
    FM_ASSERT(timer->ops != FM_NULL);
    FM_ASSERT(timer->info != FM_NULL);

    device = &(timer->parent);

    device->type = FM_Device_Class_Timer;
    device->rx_indicate = FM_NULL;
    device->tx_complete = FM_NULL;

#ifdef FM_USING_DEVICE_OPS
    device->ops = &hwtimer_ops;
#else
    device->init = fm_hwtimer_init;
    device->open = fm_hwtimer_open;
    device->close = fm_hwtimer_close;
    device->read = fm_hwtimer_read;
    device->write = fm_hwtimer_write;
    device->control = fm_hwtimer_control;
#endif
    device->user_data = user_data;

    return fm_device_register(device, name, FM_DEVICE_FLAG_RDWR | FM_DEVICE_FLAG_STANDALONE);
}
