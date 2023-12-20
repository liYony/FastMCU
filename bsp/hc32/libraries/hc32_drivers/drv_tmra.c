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
    #error "Please define at least one BSP_USING_TIM_TMRA_7"
#endif

#include <drv_tmra.h>
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
    switch (cmd)
    {
    case HWTIMER_CTRL_FREQ_SET:
    {
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

}
#endif

static void _irq_callback_config(void)
{
#ifdef BSP_USING_TIM_TMRA_1
    tim_tmra_config[TIM_TMRA_1_INDEX].irq_callback = hc32_timera1_ovf_handler;
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
    struct fm_hwtimer_info _info;

    _irq_callback_config();
    _enable_periph_clk();

    for (i = 0; i < sizeof(tim_tmra_config) / sizeof(tim_tmra_config[0]); i++)
    {
        _info = _info_get(tim_tmra_config[i].instance);
        tim_tmra_config[i].device.info = &_info;
        tim_tmra_config[i].device.ops  = &_ops;
        /* install irq */
        if (_info.cntmode != HWTIMER_CNTMODE_UP)
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
