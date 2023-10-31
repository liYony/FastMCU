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
} pwm_info_t;

static pwm_info_t pwm_info[] = 
{
    {DAL_PWM_0, CM_TMRA_2, FCG2_PERIPH_TMRA_2, CH(TMRA_CH4) | CH(TMRA_CH2)},
};

static int hc32_pwm_init(pwm_info_t *info, uint16_t div, uint32_t period)
{
    stc_tmra_init_t stcTmraInit;
    stc_tmra_pwm_init_t stcPwmInit;

    hc32_borad_pwm_init(info->unit_pwm);
    
    FCG_Fcg2PeriphClockCmd(info->fcg_clk, ENABLE);

    (void)TMRA_StructInit(&stcTmraInit);

    stcTmraInit.sw_count.u16CountMode = TMRA_MD_SAWTOOTH;
    stcTmraInit.sw_count.u16CountDir = TMRA_DIR_UP;
    stcTmraInit.sw_count.u16ClockDiv = div;
    stcTmraInit.u32PeriodValue = period - 1;
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
        if ((pwm_info->ch_mask >> i) & 0x01)
        {
            (void)TMRA_PWM_Init(info->unit_pwm, i, &stcPwmInit);
        }
    }
    
    TMRA_Start(info->unit_pwm);
    return 0;
}

int mcu_pwm_init(dal_pwm_number_t pwm, uint32_t freq, uint32_t period)
{
    hc32_pwm_init(&pwm_info[pwm], TMRA_CLK_DIV4, period);
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

int mcu_pwm_set_period(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t period)
{
    TMRA_SetPeriodValue(pwm_info[pwm].unit_pwm, period);
    return 0;
}

int mcu_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse)
{
    TMRA_SetCompareValue(pwm_info[pwm].unit_pwm, ch, pulse);
    return 0;
}

#include <qk_section.h>

void pwm_test(void)
{
    dal_pwm_init(DAL_PWM_0, 0, 4000);
    dal_pwm_set_pulse(DAL_PWM_0, DAL_PWM_CH4, 3000);
    dal_pwm_enable(DAL_PWM_0, DAL_PWM_CH4);
    dal_pwm_disable(DAL_PWM_0, DAL_PWM_CH4);
    mcu_pwm_set_period(DAL_PWM_0, DAL_PWM_CH4, 8000);
    dal_pwm_set_pulse(DAL_PWM_0, DAL_PWM_CH4, 2000);
    dal_pwm_enable(DAL_PWM_0, DAL_PWM_CH4);
}

INITLV4_EXPORT(pwm_test);
