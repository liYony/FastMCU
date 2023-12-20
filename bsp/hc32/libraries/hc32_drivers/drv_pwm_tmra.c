/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2006-2022, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-06-27     lianghongquan     first version
 * 2023-12-20     liYony            Adaptation HC32F4A0 chip
 */

#include <board.h>

#define DBG_TAG "drv.pwm.tmra"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

#ifdef BSP_USING_PWM_TMRA

#if (!(defined(BSP_USING_PWM_TMRA_1)  || defined(BSP_USING_PWM_TMRA_2)  || \
       defined(BSP_USING_PWM_TMRA_3)  || defined(BSP_USING_PWM_TMRA_4)  || \
       defined(BSP_USING_PWM_TMRA_5)  || defined(BSP_USING_PWM_TMRA_6)  || \
       defined(BSP_USING_PWM_TMRA_7)  || defined(BSP_USING_PWM_TMRA_8)  || \
       defined(BSP_USING_PWM_TMRA_9)  || defined(BSP_USING_PWM_TMRA_10) || \
       defined(BSP_USING_PWM_TMRA_11) || defined(BSP_USING_PWM_TMRA_12)))
    #error "Please define at least one BSP_USING_PWM_TMRA_7"
#endif

#include <drv_pwm_tmra.h>
#include "drv_config.h"

#define _CH(ch) (ch - 1)

extern fm_err_t fm_hw_board_pwm_tmra_init(CM_TMRA_TypeDef *TMRAx);

enum
{
#ifdef BSP_USING_PWM_TMRA_1
    PWM_TMRA_1_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_2
    PWM_TMRA_2_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_3
    PWM_TMRA_3_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_4
    PWM_TMRA_4_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_5
    PWM_TMRA_5_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_6
    PWM_TMRA_6_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_7
    PWM_TMRA_7_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_8
    PWM_TMRA_8_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_9
    PWM_TMRA_9_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_10
    PWM_TMRA_10_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_11
    PWM_TMRA_11_INDEX,
#endif
#ifdef BSP_USING_PWM_TMRA_12
    PWM_TMRA_12_INDEX,
#endif
};

static struct hc32_pwm_tmra pwm_tmra_config[] =
{
#ifdef BSP_USING_PWM_TMRA_1
    PWM_TMRA_1_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_2
    PWM_TMRA_2_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_3
    PWM_TMRA_3_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_4
    PWM_TMRA_4_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_5
    PWM_TMRA_5_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_6
    PWM_TMRA_6_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_7
    PWM_TMRA_7_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_8
    PWM_TMRA_8_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_9
    PWM_TMRA_9_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_10
    PWM_TMRA_10_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_11
    PWM_TMRA_11_CONFIG,
#endif
#ifdef BSP_USING_PWM_TMRA_12
    PWM_TMRA_12_CONFIG,
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

static fm_uint32_t get_tmra_clk_freq(CM_TMRA_TypeDef *TMRAx)
{
    fm_uint32_t u32clkFreq;
    uint16_t u16Div;
    //
    u32clkFreq = get_tmra_clk_freq_not_div(TMRAx);
    u16Div = READ_REG16(TMRAx->BCSTR) & TMRA_BCSTR_CKDIV;
    switch (u16Div)
    {
        case (TMRA_CLK_DIV2):
            u32clkFreq /= 2;
            break;
        case (TMRA_CLK_DIV4):
            u32clkFreq /= 4;
            break;
        case (TMRA_CLK_DIV8):
            u32clkFreq /= 8;
            break;
        case (TMRA_CLK_DIV16):
            u32clkFreq /= 16;
            break;
        case (TMRA_CLK_DIV32):
            u32clkFreq /= 32;
            break;
        case (TMRA_CLK_DIV64):
            u32clkFreq /= 64;
            break;
        case (TMRA_CLK_DIV128):
            u32clkFreq /= 128;
            break;
        case (TMRA_CLK_DIV256):
            u32clkFreq /= 256;
            break;
        case (TMRA_CLK_DIV512):
            u32clkFreq /= 512;
            break;
        case (TMRA_CLK_DIV1024):
            u32clkFreq /= 1024;
            break;
        case (TMRA_CLK_DIV1):
        default:
            break;
    }
    return u32clkFreq;
}

static fm_err_t drv_pwm_enable(CM_TMRA_TypeDef *TMRAx, struct fm_pwm_configuration *configuration, fm_bool_t enable)
{
    if (FM_TRUE == enable) {TMRA_PWM_OutputCmd(TMRAx, _CH(configuration->channel), ENABLE);}
    else {TMRA_PWM_OutputCmd(TMRAx, _CH(configuration->channel), DISABLE);}
    return FM_EOK;
}

static fm_err_t drv_pwm_get(CM_TMRA_TypeDef *TMRAx, struct fm_pwm_configuration *configuration)
{
    fm_uint32_t u32clkFreq;
    fm_uint64_t u64clk_ns;
    u32clkFreq = get_tmra_clk_freq(TMRAx);
    u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
    configuration->period = u64clk_ns * TMRA_GetPeriodValue(TMRAx);
    configuration->pulse = u64clk_ns * TMRA_GetCompareValue(TMRAx, _CH(configuration->channel));
    return FM_EOK;
}

static fm_err_t drv_pwm_set(CM_TMRA_TypeDef *TMRAx, struct fm_pwm_configuration *configuration)
{
    fm_uint32_t u32clkFreq;
    fm_uint64_t u64clk_ns;
    fm_uint64_t u64val;
    u32clkFreq = get_tmra_clk_freq(TMRAx);
    u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
    u64val = (fm_uint64_t)configuration->period / u64clk_ns;
    if ((configuration->period <= u64clk_ns) || (u64val > 0xFFFF))
    {
        /* clk not match, need change div */
        uint32_t div_bit;
        u32clkFreq = get_tmra_clk_freq_not_div(TMRAx);
        u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
        u64val = (fm_uint64_t)configuration->period / u64clk_ns;
        for (div_bit=0; div_bit<= 10; div_bit++)
        {
            if (u64val < 0xFFFF) break;
            u64val /= 2;
        }
        if (div_bit > 10) return -FM_ERROR;
        TMRA_SetClockDiv(TMRAx, div_bit << TMRA_BCSTR_CKDIV_POS);
        u32clkFreq = get_tmra_clk_freq(TMRAx);
        u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
    }
    TMRA_SetPeriodValue(TMRAx, configuration->period / u64clk_ns);
    TMRA_SetCompareValue(TMRAx, _CH(configuration->channel), configuration->pulse / u64clk_ns);
    return FM_EOK;
}

static fm_err_t drv_pwm_set_period(CM_TMRA_TypeDef *TMRAx, struct fm_pwm_configuration *configuration)
{
    fm_uint32_t u32clkFreq;
    fm_uint64_t u64clk_ns;
    fm_uint64_t u64val;
    u32clkFreq = get_tmra_clk_freq(TMRAx);
    u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
    u64val = (fm_uint64_t)configuration->period / u64clk_ns;
    if ((configuration->period <= u64clk_ns) || (u64val > 0xFFFF))
    {
        /* clk not match, need change div */
        uint32_t div_bit;
        u32clkFreq = get_tmra_clk_freq_not_div(TMRAx);
        u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
        u64val = (fm_uint64_t)configuration->period / u64clk_ns;
        for (div_bit=0; div_bit<= 10; div_bit++)
        {
            if (u64val < 0xFFFF) break;
            u64val /= 2;
        }
        if (div_bit > 10) return -FM_ERROR;
        TMRA_SetClockDiv(TMRAx, div_bit << TMRA_BCSTR_CKDIV_POS);
        u32clkFreq = get_tmra_clk_freq(TMRAx);
        u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
    }
    TMRA_SetPeriodValue(TMRAx, configuration->period / u64clk_ns);
    return FM_EOK;
}

static fm_err_t drv_pwm_set_pulse(CM_TMRA_TypeDef *TMRAx, struct fm_pwm_configuration *configuration)
{
    fm_uint32_t u32clkFreq;
    fm_uint64_t u64clk_ns;
    u32clkFreq = get_tmra_clk_freq(TMRAx);
    u64clk_ns = (fm_uint64_t)1000000000ul / u32clkFreq;
    TMRA_SetCompareValue(TMRAx, _CH(configuration->channel), configuration->pulse / u64clk_ns);
    return FM_EOK;
}

static fm_err_t drv_pwm_control(struct fm_device_pwm *device, int cmd, void *arg)
{
    struct fm_pwm_configuration *configuration = (struct fm_pwm_configuration *)arg;
    struct hc32_pwm_tmra *pwm;
    pwm = fm_container_of(device, struct hc32_pwm_tmra, pwm_device);
    CM_TMRA_TypeDef *TMRAx = pwm->instance;
    switch (cmd)
    {
    case PWMN_CMD_ENABLE:
        configuration->complementary = FM_TRUE;
    case PWM_CMD_ENABLE:
        return drv_pwm_enable(TMRAx, configuration, FM_TRUE);
    case PWMN_CMD_DISABLE:
        configuration->complementary = FM_FALSE;
    case PWM_CMD_DISABLE:
        return drv_pwm_enable(TMRAx, configuration, FM_FALSE);
    case PWM_CMD_SET:
        return drv_pwm_set(TMRAx, configuration);
    case PWM_CMD_GET:
        return drv_pwm_get(TMRAx, configuration);
    case PWM_CMD_SET_PERIOD:
        return drv_pwm_set_period(TMRAx, configuration);
    case PWM_CMD_SET_PULSE:
        return drv_pwm_set_pulse(TMRAx, configuration);
    default:
        return -FM_EINVAL;
    }
}

static fm_err_t _pwm_tmra_init(struct hc32_pwm_tmra *device)
{
    CM_TMRA_TypeDef *TMRAx;
    uint32_t i;
    FM_ASSERT(device != FM_NULL);
    TMRAx = device->instance;
    TMRA_Init(TMRAx, &device->stcTmraInit);
    for (i=0; i<8; i++)
    {
        if ((device->channel >> i) & 0x01)
        {
            TMRA_PWM_Init(TMRAx, i, &device->stcPwmInit);
        }
    }
    TMRA_Start(TMRAx);
    return FM_EOK;
}

static void pwm_get_channel(void)
{
#ifdef BSP_USING_PWM_TMRA_1
    #ifdef BSP_USING_PWM_TMRA_1_CH1
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH2
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH3
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH4
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH5
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH6
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH7
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_1_CH8
        pwm_tmra_config[PWM_TMRA_1_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_2
    #ifdef BSP_USING_PWM_TMRA_2_CH1
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH2
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH3
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH4
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH5
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH6
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH7
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_2_CH8
        pwm_tmra_config[PWM_TMRA_2_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_3
    #ifdef BSP_USING_PWM_TMRA_3_CH1
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH2
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH3
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH4
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH5
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH6
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH7
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_3_CH8
        pwm_tmra_config[PWM_TMRA_3_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_4
    #ifdef BSP_USING_PWM_TMRA_4_CH1
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH2
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH3
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH4
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH5
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH6
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH7
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_4_CH8
        pwm_tmra_config[PWM_TMRA_4_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_5
    #ifdef BSP_USING_PWM_TMRA_5_CH1
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH2
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH3
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH4
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH5
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH6
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH7
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_5_CH8
        pwm_tmra_config[PWM_TMRA_5_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_6
    #ifdef BSP_USING_PWM_TMRA_6_CH1
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH2
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH3
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH4
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH5
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH6
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH7
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_6_CH8
        pwm_tmra_config[PWM_TMRA_6_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_7
    #ifdef BSP_USING_PWM_TMRA_7_CH1
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH2
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH3
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH4
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH5
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH6
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH7
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_7_CH8
        pwm_tmra_config[PWM_TMRA_7_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_8
    #ifdef BSP_USING_PWM_TMRA_8_CH1
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH2
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH3
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH4
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH5
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH6
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH7
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_8_CH8
        pwm_tmra_config[PWM_TMRA_8_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_9
    #ifdef BSP_USING_PWM_TMRA_9_CH1
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH2
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH3
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH4
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH5
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH6
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH7
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_9_CH8
        pwm_tmra_config[PWM_TMRA_9_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_10
    #ifdef BSP_USING_PWM_TMRA_10_CH1
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH2
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH3
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH4
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH5
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH6
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH7
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_10_CH8
        pwm_tmra_config[PWM_TMRA_10_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_11
    #ifdef BSP_USING_PWM_TMRA_11_CH1
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH2
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH3
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH4
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH5
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH6
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH7
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_11_CH8
        pwm_tmra_config[PWM_TMRA_11_INDEX].channel |= 1 << 7;
    #endif
#endif
#ifdef BSP_USING_PWM_TMRA_12
    #ifdef BSP_USING_PWM_TMRA_12_CH1
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 0;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH2
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 1;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH3
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 2;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH4
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 3;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH5
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 4;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH6
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 5;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH7
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 6;
    #endif
    #ifdef BSP_USING_PWM_TMRA_12_CH8
        pwm_tmra_config[PWM_TMRA_12_INDEX].channel |= 1 << 7;
    #endif
#endif
}

static void _enable_periph_clk(void)
{
#ifdef BSP_USING_PWM_TMRA_1
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_1, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_2
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_2, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_3
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_3, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_4
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_4, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_5
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_5, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_6
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_6, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_7
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_7, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_8
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_8, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_9
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_9, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_10
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_10, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_11
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_11, ENABLE);
#endif
#ifdef BSP_USING_PWM_TMRA_12
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_12, ENABLE);
#endif
}

static struct fm_pwm_ops drv_ops =
{
    drv_pwm_control
};

static int fm_hw_pwm_tmra_init(void)
{
    int i = 0;
    fm_err_t result = FM_EOK;

    pwm_get_channel();
    _enable_periph_clk();
    for (i = 0; i < sizeof(pwm_tmra_config) / sizeof(pwm_tmra_config[0]); i++)
    {
        /* pwm init */
        _pwm_tmra_init(&pwm_tmra_config[i]);
        fm_hw_board_pwm_tmra_init(pwm_tmra_config[i].instance);
        /* register UART device */
        result = fm_device_pwm_register(&pwm_tmra_config[i].pwm_device, pwm_tmra_config[i].name, &drv_ops, &pwm_tmra_config[i].instance);
        FM_ASSERT(result == FM_EOK);
    }
    return result;
}

INIT_DEVICE_EXPORT(fm_hw_pwm_tmra_init);

#endif /* BSP_USING_PWM_TMRA */
