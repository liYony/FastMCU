#ifndef __TIM_CONFIG_H__
#define __TIM_CONFIG_H__

#include <fmcu.h>
#include "irq_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_USING_TIM_TMRA_1
#ifndef TIM_TMRA_1_CONFIG
#define TIM_TMRA_1_CONFIG                                       \
    {                                                           \
        .name           = "tim_a1",                             \
        .instance       = CM_TMRA_1,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_1_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_1_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_1_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_CONFIG */
#endif /* BSP_USING_TIM_TMRA_1 */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_CONFIG_H__ */
