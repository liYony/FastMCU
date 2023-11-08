#include <dal.h>
#include "hc32_ll.h"

#define TMRA_IRQN_BASE (INT030_IRQn)

typedef struct
{
    dal_timer_number_t timer;
    CM_TMRA_TypeDef *unit_tmra;
    en_int_src_t int_src_b;
    func_ptr_t cb_fn;
    uint32_t fcg_clk;
    uint8_t prio;
    uint32_t clk_freq;
    float tick_ns;
} timera_info_t;

static void timera1_callback(void);
static void timera5_callback(void);

static timera_info_t timera_info[] =
{
    {DAL_TIMER_0, CM_TMRA_1, INT_SRC_TMRA_1_OVF, timera1_callback, FCG2_PERIPH_TMRA_1, DDL_IRQ_PRIO_02},
    {DAL_TIMER_1, CM_TMRA_5, INT_SRC_TMRA_5_OVF, timera5_callback, FCG2_PERIPH_TMRA_5, DDL_IRQ_PRIO_02},
};

static void timera_info_calc_clk(timera_info_t *info)
{
    stc_clock_freq_t stcClkFreq;
    CLK_GetClockFreq(&stcClkFreq);

    if ((info->fcg_clk & 0x11000000UL))
    {
        info->clk_freq = stcClkFreq.u32Pclk1Freq;
        log_i("DAL_TIMER%d attach in %s, freq is %dMHz", info->timer, "CLK1", info->clk_freq / 1000000);
    }
    else
    {
        info->clk_freq = stcClkFreq.u32Pclk0Freq;
        log_i("DAL_TIMER%d attach in %s, freq is %dMHz", info->timer, "CLK0", info->clk_freq / 1000000);
    }
}

static void timera_init(timera_info_t *info, dal_timer_cntmode_t cntm, uint16_t clk_div)
{
    stc_tmra_init_t stcTmraInit;

    /* 1. Enable TimerA peripheral clock. */
    FCG_Fcg2PeriphClockCmd(info->fcg_clk, ENABLE);

    /* 2. Set a default initialization value for stcTmraInit. */
    (void)TMRA_StructInit(&stcTmraInit);

    /* 3. Modifies the initialization values depends on the application. */
    stcTmraInit.sw_count.u16ClockDiv = clk_div;
    stcTmraInit.sw_count.u16CountMode = TMRA_MD_SAWTOOTH;
    if (cntm == DAL_TIMER_CNTMODE_UP)
    {
        stcTmraInit.sw_count.u16CountDir = TMRA_DIR_UP;
        info->int_src_b = info->int_src_b;
    }
    else
    {
        stcTmraInit.sw_count.u16CountDir = TMRA_DIR_DOWN;
        info->int_src_b = (en_int_src_t)(info->int_src_b + 1);
    }
    stcTmraInit.u32PeriodValue = 65535;

    (void)TMRA_Init(info->unit_tmra, &stcTmraInit);

    /* 4. Configures IRQ if needed. */
    stc_irq_signin_config_t stcIrq;

    stcIrq.enIntSrc = info->int_src_b;
    stcIrq.enIRQn = (IRQn_Type)(TMRA_IRQN_BASE + info->timer);
    stcIrq.pfnCallback = info->cb_fn;
    (void)INTC_IrqSignIn(&stcIrq);

    NVIC_ClearPendingIRQ(stcIrq.enIRQn);
    NVIC_SetPriority(stcIrq.enIRQn, info->prio);
    NVIC_EnableIRQ(stcIrq.enIRQn);

    /* Enable the specified interrupts of TimerA. */
    if (cntm == DAL_TIMER_CNTMODE_UP)
    {
        TMRA_IntCmd(info->unit_tmra, TMRA_INT_OVF, ENABLE);
    }
    else
    {
        TMRA_IntCmd(info->unit_tmra, TMRA_INT_UDF, ENABLE);
    }
}

static void timera1_callback(void)
{
    //    TMRA_ClearStatus(CM_TMRA_1, TMRA_INT_UDF);
    dal_it_invoke(DAL_HAL_IT_TIMER_FLOW, DAL_TIMER_0, NULL);
    log_d("timera1 irq %d", dal_get_systick());
}

static void timera5_callback(void)
{
    //    TMRA_ClearStatus(CM_TMRA_5, TMRA_INT_UDF);
    dal_it_invoke(DAL_HAL_IT_TIMER_FLOW, DAL_TIMER_5, NULL);
    log_d("timera5 irq");
}

int mcu_timer_init(dal_timer_number_t timer, dal_timer_cntmode_t cntm, uint32_t time_max_ns)
{
    if (timer >= (sizeof(timera_info) / sizeof(timera_info[0])))
    {
        return -1;
    }

    timera_info_calc_clk(&timera_info[timer]);

    float clk_ns = 1000000000.f / (float)timera_info[timer].clk_freq;
    uint32_t tick = time_max_ns / clk_ns;
    
    uint8_t div_bit = 0;
    for (div_bit=0; div_bit<= 10; div_bit++)
    {
        if (tick < 0xFFFF) break;
        tick /= 2;
    }
    if (div_bit > 10)
    {
        log_e("div%d not support.", div_bit);
        return -1;
    }

    timera_info[timer].clk_freq = (uint32_t)(timera_info[timer].clk_freq / (1 << div_bit));
    timera_info[timer].tick_ns = (float)(1000000000.f / timera_info[timer].clk_freq);
    
    log_d("One tick takes (%.2fns)", timera_info[timer].tick_ns);
    timera_init(&timera_info[timer], cntm, (div_bit << TMRA_BCSTR_CKDIV_POS));
    
    return 0;
}

int mcu_timer_start(dal_timer_number_t timer, dal_timer_mode_t mode, uint32_t time_ns)
{
    if (timer >= (sizeof(timera_info) / sizeof(timera_info[0])))
    {
        return -1;
    }
    
    uint16_t tick = 0;
    
    tick = (uint16_t)(time_ns / timera_info[timer].tick_ns);
    log_d("%dns need to use %d ticks.", time_ns, tick);
    TMRA_SetPeriodValue(timera_info[timer].unit_tmra, tick);
    TMRA_SetCountValue(timera_info[timer].unit_tmra, 0);
    TMRA_Start(timera_info[timer].unit_tmra);
    
    return 0;
}

int mcu_timer_stop(dal_timer_number_t timer)
{
    if (timer >= (sizeof(timera_info) / sizeof(timera_info[0])))
    {
        return -1;
    }
    TMRA_Stop(timera_info[timer].unit_tmra);
    
    return 0;
}
