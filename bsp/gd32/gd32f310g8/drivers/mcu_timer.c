#include <dal.h>
#include "gd32f3x0.h"

static void set_timerx_freq(uint32_t timerx, uint32_t freq)
{
    uint32_t ap1freq, ap2freq;
    uint16_t prescaler;
    uint32_t temp;

    if (timerx >= TIMER0)
    {
        ap2freq = rcu_clock_freq_get(CK_APB2);
        temp = RCU_CFG0 & RCU_CFG0_APB2PSC;
        temp >>= 11;
        /* whether should frequency doubling */
        temp = (temp < 4) ? 0 : 1;

        prescaler = (ap2freq << temp) / freq - 1;
        
        log_d("ap2freq = %d, prescaler = %d\r\n", ap2freq, prescaler);
    }
    else
    {
        ap1freq = rcu_clock_freq_get(CK_APB1);
        temp = RCU_CFG0 & RCU_CFG0_APB1PSC;
        temp >>= 8;
        /* whether should frequency doubling */
        temp = (temp < 4) ? 0 : 1;

        prescaler = (ap1freq << temp) / freq - 1;
        
        log_d("ap1freq = %d, prescaler = %d\r\n", ap1freq, prescaler);
    }
    
    timer_prescaler_config(timerx, prescaler, TIMER_PSC_RELOAD_NOW);
}

void 

void mcu_timer_init(dal_timer_number_t timer)
{
    if (timer == DAL_TIMER_15)
    {
        timer_internal_clock_config(timer_base);
    }
    
    
//    
//    timer_internal_clock_config(timer_base);
//    timer_struct_para_init(&initpara);
//    initpara.period =  timer->info->maxcnt;
//    timer_init(timer_base, &initpara);
//    set_timerx_freq(timer_base, timer->info->maxfreq);
}

#include <qk_section.h>


void timer_test(void)
{
//    set_timerx_freq(TIMER15, 1000000);
    mcu_timer_init(DAL_TIMER_0);
    mcu_timer_init(DAL_TIMER_1);
    mcu_timer_init(DAL_TIMER_2);
    mcu_timer_init(DAL_TIMER_13);
    mcu_timer_init(DAL_TIMER_14);
    mcu_timer_init(DAL_TIMER_15);
    mcu_timer_init(DAL_TIMER_16);
}

INITLV4_EXPORT(timer_test);
