/*
 * Copyright (c) 2006-2022, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2023-12-21     liYony            first version
 */

#include <board.h>

#define DBG_TAG "drv.tim.tmra"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

#ifdef BSP_USING_TIM_TMRA

#if (!(defined(BSP_USING_TIM_TMRA_1)  || defined(BSP_USING_TIM_TMRA_2)  || \
       defined(BSP_USING_TIM_TMRA_3)  || defined(BSP_USING_TIM_TMRA_4)  || \
       defined(BSP_USING_TIM_TMRA_5)  || defined(BSP_USING_TIM_TMRA_6)  || \
       defined(BSP_USING_TIM_TMRA_7)  || defined(BSP_USING_TIM_TMRA_8)  || \
       defined(BSP_USING_TIM_TMRA_9)  || defined(BSP_USING_TIM_TMRA_10) || \
       defined(BSP_USING_TIM_TMRA_11) || defined(BSP_USING_TIM_TMRA_12)))
    #error "Please define at least one BSP_USING_TIM_TMRA_x"
#endif

#include <drv_tim_tmra.h>
#include "drv_config.h"

enum
{
#ifdef BSP_USING_TIM_TMRA_1
    TIM_TMRA_1_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_2
    TIM_TMRA_2_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_3
    TIM_TMRA_3_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_4
    TIM_TMRA_4_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_5
    TIM_TMRA_5_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_6
    TIM_TMRA_6_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_7
    TIM_TMRA_7_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_8
    TIM_TMRA_8_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_9
    TIM_TMRA_9_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_10
    TIM_TMRA_10_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_11
    TIM_TMRA_11_INDEX,
#endif
#ifdef BSP_USING_TIM_TMRA_12
    TIM_TMRA_12_INDEX,
#endif
};

static struct hc32_tim_tmra tim_tmra_config[] =
{
#ifdef BSP_USING_TIM_TMRA_1
    TIM_TMRA_1_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_2
    TIM_TMRA_2_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_3
    TIM_TMRA_3_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_4
    TIM_TMRA_4_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_5
    TIM_TMRA_5_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_6
    TIM_TMRA_6_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_7
    TIM_TMRA_7_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_8
    TIM_TMRA_8_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_9
    TIM_TMRA_9_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_10
    TIM_TMRA_10_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_11
    TIM_TMRA_11_CONFIG,
#endif
#ifdef BSP_USING_TIM_TMRA_12
    TIM_TMRA_12_CONFIG,
#endif
};

static fm_uint32_t get_tmra_clk_freq_not_div(CM_TMRA_TypeDef *TMRAx)
{
    stc_clock_freq_t stcClockFreq;
    CLK_GetClockFreq(&stcClockFreq);
    
#if defined (HC32F4A0)
    if ((fm_uint32_t)TMRAx <= CM_TMRA_12_BASE)
    {
        return stcClockFreq.u32Pclk1Freq;
    }
    else
    {
        return stcClockFreq.u32Pclk0Freq;
    }
#elif defined (HC32F460)
    return stcClockFreq.u32Pclk1Freq;
#endif
}

static struct fm_hwtimer_info _info_get(CM_TMRA_TypeDef *TMRAx)
{
    struct fm_hwtimer_info _info;
    fm_uint32_t clk_freq = get_tmra_clk_freq_not_div(TMRAx);
    
    _info.maxfreq = clk_freq / 1;
    _info.minfreq = clk_freq / 1024;
    _info.maxcnt = 0xFFFF;
    _info.cntmode = HWTIMER_CNTMODE_UP;
    
    return _info;
}

static void hc32_timer_init(struct fm_hwtimer_device *timer, fm_uint32_t state)
{
    CM_TMRA_TypeDef *tmra_ins;
    stc_tmra_init_t stcTmraInit;
    struct hc32_tim_tmra *tim_device;

    FM_ASSERT(FM_NULL != timer);
    if (state)
    {
        tmra_ins = (CM_TMRA_TypeDef *)timer->parent.user_data;
        tim_device = (struct hc32_tim_tmra *)timer;

        (void)TMRA_StructInit(&stcTmraInit);
        
        stcTmraInit.sw_count.u16ClockDiv = TMRA_CLK_DIV1024;
        stcTmraInit.sw_count.u16CountMode = TMRA_MD_SAWTOOTH;
        if (timer->info->cntmode == HWTIMER_CNTMODE_UP)
        {
            stcTmraInit.sw_count.u16CountDir = TMRA_DIR_UP;
        }
        else
        {
            stcTmraInit.sw_count.u16CountDir = TMRA_DIR_DOWN;
        }
        stcTmraInit.u32PeriodValue = timer->info->maxcnt;
        
        (void)TMRA_Init(tmra_ins, &stcTmraInit);
        
        NVIC_EnableIRQ(tim_device->irq_config.irq_num);
        if (timer->info->cntmode == HWTIMER_CNTMODE_UP)
        {
            TMRA_IntCmd(tmra_ins, TMRA_INT_OVF, ENABLE);
        }
        else
        {
            TMRA_IntCmd(tmra_ins, TMRA_INT_UDF, ENABLE);
        }
    }
}
static fm_err_t hc32_timer_start(fm_hwtimer_t *timer, fm_uint32_t t, fm_hwtimer_mode_t opmode)
{
    CM_TMRA_TypeDef *tmra_ins;

    FM_ASSERT(FM_NULL != timer);
    tmra_ins = (CM_TMRA_TypeDef *)timer->parent.user_data;

    (void)opmode; // not support

    TMRA_SetPeriodValue(tmra_ins, t);
    TMRA_SetCountValue(tmra_ins, 0);
    TMRA_Start(tmra_ins);
    
    return FM_EOK;
}
static void hc32_timer_stop(fm_hwtimer_t *timer)
{
    CM_TMRA_TypeDef *tmra_ins;

    FM_ASSERT(FM_NULL != timer);
    tmra_ins = (CM_TMRA_TypeDef *)timer->parent.user_data;

    TMRA_Stop(tmra_ins);
    TMRA_SetCountValue(tmra_ins, 0);
}
static fm_err_t hc32_timer_ctrl(fm_hwtimer_t *timer, fm_uint32_t cmd, void *arg)
{
    fm_err_t result = -FM_ERROR;
    struct hc32_tim_tmra *tim_device;
    tim_device = (struct hc32_tim_tmra *)timer;

    switch (cmd)
    {
    case HWTIMER_CTRL_FREQ_SET:
    {
        fm_uint32_t sfreq = *((fm_uint32_t *)arg);
        fm_uint32_t nfreq = tim_device->clk_freq;

        int div_bit = 0;
        for (div_bit = 0; div_bit <= 10; div_bit++)
        {
            if (sfreq >= tim_device->clk_freq) break;
            sfreq *= 2;
            nfreq /= 2;
        }
        if (div_bit > 10)
        {
            LOG_E("not support %dHz.", sfreq);
            return -FM_ERROR;
        }
        
        if (*((fm_uint32_t *)arg) != nfreq)
        {
            LOG_W("Freq %dHz not support, change to %dHz.", *((fm_uint32_t *)arg), nfreq);
            timer->freq = nfreq;
        }
        
        TMRA_SetClockDiv(tim_device->instance, (div_bit << TMRA_BCSTR_CKDIV_POS));
    }
    break;
    default:
    {
        result = -FM_EINVAL;
    }
    break;
    }
    
    return result;
}
static fm_uint32_t hc32_timer_counter_get(fm_hwtimer_t *timer)
{
    CM_TMRA_TypeDef *tmra_ins;

    FM_ASSERT(FM_NULL != timer);
    tmra_ins = (CM_TMRA_TypeDef *)timer->parent.user_data;

    return TMRA_GetCountValue(tmra_ins);
}

#ifdef BSP_USING_TIM_TMRA_1
static void hc32_timera1_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_1_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_2
static void hc32_timera2_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_2_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_3
static void hc32_timera3_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_3_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_4
static void hc32_timera4_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_4_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_5
static void hc32_timera5_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_5_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_6
static void hc32_timera6_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_6_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_7
static void hc32_timera7_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_7_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_8
static void hc32_timera8_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_8_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_9
static void hc32_timera9_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_9_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_10
static void hc32_timera10_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_10_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_11
static void hc32_timera11_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_11_INDEX].device);
}
#endif
#ifdef BSP_USING_TIM_TMRA_12
static void hc32_timera12_ovf_handler(void)
{
    fm_device_hwtimer_isr(&tim_tmra_config[TIM_TMRA_12_INDEX].device);
}
#endif
static void _irq_callback_config(void)
{
#ifdef BSP_USING_TIM_TMRA_1
    tim_tmra_config[TIM_TMRA_1_INDEX].irq_callback = hc32_timera1_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_2
    tim_tmra_config[TIM_TMRA_2_INDEX].irq_callback = hc32_timera2_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_3
    tim_tmra_config[TIM_TMRA_3_INDEX].irq_callback = hc32_timera3_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_4
    tim_tmra_config[TIM_TMRA_4_INDEX].irq_callback = hc32_timera4_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_5
    tim_tmra_config[TIM_TMRA_5_INDEX].irq_callback = hc32_timera5_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_6
    tim_tmra_config[TIM_TMRA_6_INDEX].irq_callback = hc32_timera6_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_7
    tim_tmra_config[TIM_TMRA_7_INDEX].irq_callback = hc32_timera7_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_8
    tim_tmra_config[TIM_TMRA_8_INDEX].irq_callback = hc32_timera8_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_9
    tim_tmra_config[TIM_TMRA_9_INDEX].irq_callback = hc32_timera9_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_10
    tim_tmra_config[TIM_TMRA_10_INDEX].irq_callback = hc32_timera10_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_11
    tim_tmra_config[TIM_TMRA_11_INDEX].irq_callback = hc32_timera11_ovf_handler;
#endif
#ifdef BSP_USING_TIM_TMRA_12
    tim_tmra_config[TIM_TMRA_12_INDEX].irq_callback = hc32_timera12_ovf_handler;
#endif
}

static void _enable_periph_clk(void)
{
#ifdef BSP_USING_TIM_TMRA_1
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_1, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_2
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_2, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_3
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_3, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_4
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_4, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_5
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_5, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_6
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_6, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_7
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_7, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_8
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_8, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_9
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_9, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_10
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_10, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_11
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_11, ENABLE);
#endif
#ifdef BSP_USING_TIM_TMRA_12
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_12, ENABLE);
#endif
}

static const struct fm_hwtimer_ops _ops =
{
    .init = hc32_timer_init,
    .start = hc32_timer_start,
    .stop = hc32_timer_stop,
    .count_get = hc32_timer_counter_get,
    .control = hc32_timer_ctrl,
};

static int fm_hw_timer_tmra_init(void)
{
    int i = 0;
    int result = FM_EOK;
    struct fm_hwtimer_info *_info;

    _irq_callback_config();
    _enable_periph_clk();

    for (i = 0; i < sizeof(tim_tmra_config) / sizeof(tim_tmra_config[0]); i++)
    {
        _info = (struct fm_hwtimer_info *)fm_malloc(sizeof(struct fm_hwtimer_info));
        FM_ASSERT(_info != FM_NULL);
        *_info = _info_get(tim_tmra_config[i].instance);
        tim_tmra_config[i].device.info = _info;
        tim_tmra_config[i].device.ops  = &_ops;
        tim_tmra_config[i].clk_freq = _info->maxfreq;
        /* install irq */
        if (_info->cntmode != HWTIMER_CNTMODE_UP)
        {
            /* INT_SRC_TMRA_x_OVF --> INT_SRC_TMRA_x_UDF */
            tim_tmra_config[i].irq_config.int_src += 1;
        }
        hc32_install_irq_handler(&tim_tmra_config[i].irq_config, tim_tmra_config[i].irq_callback, FM_FALSE);
        /* register device */
        if (fm_device_hwtimer_register(&tim_tmra_config[i].device,
            tim_tmra_config[i].name, tim_tmra_config[i].instance) == FM_EOK)
        {
            LOG_D("%s register success", tim_tmra_config[i].name);
        }
        else
        {
            LOG_E("%s register failed", tim_tmra_config[i].name);
            result = -FM_ERROR;
        }
    }

    return result;
}

INIT_BOARD_EXPORT(fm_hw_timer_tmra_init);

#endif /* BSP_USING_TIM_TMRA */
