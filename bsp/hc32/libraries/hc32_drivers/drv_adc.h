/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 * 2023-12-20     liYony       Adaptive FastMCU
 */


#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <fmcu.h>
#include "fmdevice.h"

#include "hc32_ll.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
struct adc_dev_init_params
{
    char name[8];
    uint16_t resolution;                /*!< Specifies the ADC resolution.
                                             This parameter can be a value of @ref ADC_Resolution */
    uint16_t data_align;                /*!< Specifies ADC data alignment.
                                             This parameter can be a value of @ref ADC_Data_Align */
    fm_bool_t hard_trig_enable;

    uint32_t hard_trig_src;             /*a value of @ref ADC_Hard_Trigger_Sel */
    fm_bool_t internal_trig0_comtrg0_enable;
    fm_bool_t internal_trig0_comtrg1_enable;
    en_event_src_t internal_trig0_sel;         /*@ref en_event_src_t in details */
    fm_bool_t internal_trig1_comtrg0_enable;
    fm_bool_t internal_trig1_comtrg1_enable;
    en_event_src_t internal_trig1_sel;         /*@ref en_event_src_t in details */

    fm_bool_t continue_conv_mode_enable;
    fm_bool_t data_reg_auto_clear;
    uint32_t eoc_poll_time_max;
};

typedef struct
{
    struct fm_adc_device fm_adc;
    CM_ADC_TypeDef *instance;
    struct adc_dev_init_params init;
} adc_device;

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
 * Global function prototypes (definition in C source)
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __DRV_ADC_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
