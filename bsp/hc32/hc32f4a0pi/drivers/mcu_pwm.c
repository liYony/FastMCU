#include <dal.h>
#include "hc32_ll.h"
#include <board_config.h>

#define CH(ch) (1 << ch)

typedef struct
{
    dal_pwm_number_t pwm;
    CM_TMRA_TypeDef *unit_pwm;
    uint32_t fcg_clk;
    uint32_t ch_mask;
    
    uint32_t clk_freq;
    float tick_ns;
} pwm_info_t;

static pwm_info_t pwm_info[] = 
{
    {DAL_PWM_0, CM_TMRA_2,  FCG2_PERIPH_TMRA_2,  CH(TMRA_CH4)},
    {DAL_PWM_1, CM_TMRA_12, FCG2_PERIPH_TMRA_12, CH(TMRA_CH2)},
    {DAL_PWM_2, CM_TMRA_10, FCG2_PERIPH_TMRA_10, CH(TMRA_CH2)},
};

static void timera_info_calc_clk(pwm_info_t *info)
{
    stc_clock_freq_t stcClkFreq;
    CLK_GetClockFreq(&stcClkFreq);

    if ((info->fcg_clk & 0x11000000UL))
    {
        info->clk_freq = stcClkFreq.u32Pclk1Freq;
        log_i("DAL_PWM_%d attach in %s, freq is %dMHz", info->pwm, "CLK1", info->clk_freq / 1000000);
    }
    else
    {
        info->clk_freq = stcClkFreq.u32Pclk0Freq;
        log_i("DAL_PWM_%d attach in %s, freq is %dMHz", info->pwm, "CLK0", info->clk_freq / 1000000);
    }
}

static int hc32_pwm_init(pwm_info_t *info)
{
    stc_tmra_init_t stcTmraInit;
    stc_tmra_pwm_init_t stcPwmInit;

    hc32_borad_pwm_init(info->unit_pwm);
    
    FCG_Fcg2PeriphClockCmd(info->fcg_clk, ENABLE);

    (void)TMRA_StructInit(&stcTmraInit);
    stcTmraInit.sw_count.u16CountMode = TMRA_MD_SAWTOOTH;
    stcTmraInit.sw_count.u16CountDir = TMRA_DIR_UP;
    (void)TMRA_Init(info->unit_pwm, &stcTmraInit);

    (void)TMRA_PWM_StructInit(&stcPwmInit);
    stcPwmInit.u32CompareValue = 0;
    stcPwmInit.u16StartPolarity = TMRA_PWM_LOW;
    stcPwmInit.u16StopPolarity = TMRA_PWM_LOW;
    stcPwmInit.u16CompareMatchPolarity = TMRA_PWM_HIGH;
    stcPwmInit.u16PeriodMatchPolarity = TMRA_PWM_LOW;
    
    uint8_t i = 0;
    for (i=0; i<8; i++)
    {
        if ((info->ch_mask >> i) & 0x01)
        {
            (void)TMRA_PWM_Init(info->unit_pwm, i, &stcPwmInit);
        }
    }
    
    TMRA_Start(info->unit_pwm);
    return 0;
}

int mcu_pwm_init(dal_pwm_number_t pwm, uint32_t period)
{
    hc32_pwm_init(&pwm_info[pwm]);
    mcu_pwm_set_period(pwm, period);
    return 0;
}

int mcu_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    TMRA_PWM_OutputCmd(pwm_info[pwm].unit_pwm, ch, ENABLE);
    return 0;
}

int mcu_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    TMRA_PWM_OutputCmd(pwm_info[pwm].unit_pwm, ch, DISABLE);
    return 0;
}

int mcu_pwm_set_period(dal_pwm_number_t pwm, uint32_t period)
{
    timera_info_calc_clk(&pwm_info[pwm]);
    
    float clk_ns = 1000000000.f / (float)pwm_info[pwm].clk_freq;
    uint32_t tick = period / clk_ns;
    
    uint8_t div_bit = 0;
    for (div_bit=0; div_bit<= 10; div_bit++)
    {
        if (tick < 0xFFFF) 
        {
            log_i("using div%d.", div_bit);
            break;
        }
        tick /= 2;
    }
    if (div_bit > 10)
    {
        log_e("div%d not support.", div_bit);
        return -1;
    }
    
    pwm_info[pwm].clk_freq = (uint32_t)(pwm_info[pwm].clk_freq / (1 << div_bit));
    pwm_info[pwm].tick_ns = (float)(1000000000.f / pwm_info[pwm].clk_freq);
    
    log_d("One tick takes (%.2fns), all (%d) ticks.", pwm_info[pwm].tick_ns, tick);
    
    TMRA_SetClockDiv(pwm_info[pwm].unit_pwm, div_bit << TMRA_BCSTR_CKDIV_POS);
    TMRA_SetPeriodValue(pwm_info[pwm].unit_pwm, tick - 1);
    
    return 0;
}

uint32_t mcu_pwm_get_period(dal_pwm_number_t pwm)
{
    return (TMRA_GetPeriodValue(pwm_info[pwm].unit_pwm) + 1) * pwm_info[pwm].tick_ns + 0.5f;
}

int mcu_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse)
{
    uint32_t _pulse = pulse / pwm_info[pwm].tick_ns;
    TMRA_SetCompareValue(pwm_info[pwm].unit_pwm, ch, _pulse);
    return 0;
}

uint32_t mcu_pwm_get_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return TMRA_GetCompareValue(pwm_info[pwm].unit_pwm, ch) * pwm_info[pwm].tick_ns + 0.5f;
}
