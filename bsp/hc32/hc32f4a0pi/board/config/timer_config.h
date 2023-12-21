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

#ifdef BSP_USING_TIM_TMRA_2
#ifndef TIM_TMRA_2_CONFIG
#define TIM_TMRA_2_CONFIG                                       \
    {                                                           \
        .name           = "tim_a2",                             \
        .instance       = CM_TMRA_2,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_2_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_2_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_2_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_2_CONFIG */
#endif /* BSP_USING_TIM_TMRA_2 */

#ifdef BSP_USING_TIM_TMRA_3
#ifndef TIM_TMRA_3_CONFIG
#define TIM_TMRA_3_CONFIG                                       \
    {                                                           \
        .name           = "tim_a3",                             \
        .instance       = CM_TMRA_3,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_3_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_3_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_3_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_3_CONFIG */
#endif /* BSP_USING_TIM_TMRA_3 */

#ifdef BSP_USING_TIM_TMRA_4
#ifndef TIM_TMRA_4_CONFIG
#define TIM_TMRA_4_CONFIG                                       \
    {                                                           \
        .name           = "tim_a4",                             \
        .instance       = CM_TMRA_4,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_4_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_4_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_4_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_4_CONFIG */
#endif /* BSP_USING_TIM_TMRA_4 */

#ifdef BSP_USING_TIM_TMRA_5
#ifndef TIM_TMRA_5_CONFIG
#define TIM_TMRA_5_CONFIG                                       \
    {                                                           \
        .name           = "tim_a5",                             \
        .instance       = CM_TMRA_5,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_5_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_5_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_5_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_5_CONFIG */
#endif /* BSP_USING_TIM_TMRA_5 */

#ifdef BSP_USING_TIM_TMRA_6
#ifndef TIM_TMRA_6_CONFIG
#define TIM_TMRA_6_CONFIG                                       \
    {                                                           \
        .name           = "tim_a6",                             \
        .instance       = CM_TMRA_6,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_6_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_6_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_6_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_6_CONFIG */
#endif /* BSP_USING_TIM_TMRA_6 */

#ifdef BSP_USING_TIM_TMRA_7
#ifndef TIM_TMRA_7_CONFIG
#define TIM_TMRA_7_CONFIG                                       \
    {                                                           \
        .name           = "tim_a7",                             \
        .instance       = CM_TMRA_7,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_7_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_7_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_7_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_7_CONFIG */
#endif /* BSP_USING_TIM_TMRA_7 */

#ifdef BSP_USING_TIM_TMRA_8
#ifndef TIM_TMRA_8_CONFIG
#define TIM_TMRA_8_CONFIG                                       \
    {                                                           \
        .name           = "tim_a8",                             \
        .instance       = CM_TMRA_8,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_8_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_8_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_8_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_8_CONFIG */
#endif /* BSP_USING_TIM_TMRA_8 */

#ifdef BSP_USING_TIM_TMRA_9
#ifndef TIM_TMRA_9_CONFIG
#define TIM_TMRA_9_CONFIG                                       \
    {                                                           \
        .name           = "tim_a9",                             \
        .instance       = CM_TMRA_9,                            \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_9_FLOW_IRQ_NUM,              \
            .irq_prio   = TIM_TMRA_9_FLOW_IRQ_PRIO,             \
            .int_src    = INT_SRC_TMRA_9_OVF,                   \
        },                                                      \
    }
#endif /* TIM_TMRA_9_CONFIG */
#endif /* BSP_USING_TIM_TMRA_9 */

#ifdef BSP_USING_TIM_TMRA_10
#ifndef TIM_TMRA_10_CONFIG
#define TIM_TMRA_10_CONFIG                                      \
    {                                                           \
        .name           = "tim_a10",                            \
        .instance       = CM_TMRA_10,                           \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_10_FLOW_IRQ_NUM,             \
            .irq_prio   = TIM_TMRA_10_FLOW_IRQ_PRIO,            \
            .int_src    = INT_SRC_TMRA_10_OVF,                  \
        },                                                      \
    }
#endif /* TIM_TMRA_10_CONFIG */
#endif /* BSP_USING_TIM_TMRA_10 */

#ifdef BSP_USING_TIM_TMRA_11
#ifndef TIM_TMRA_11_CONFIG
#define TIM_TMRA_11_CONFIG                                      \
    {                                                           \
        .name           = "tim_a11",                            \
        .instance       = CM_TMRA_11,                           \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_11_FLOW_IRQ_NUM,             \
            .irq_prio   = TIM_TMRA_11_FLOW_IRQ_PRIO,            \
            .int_src    = INT_SRC_TMRA_11_OVF,                  \
        },                                                      \
    }
#endif /* TIM_TMRA_11_CONFIG */
#endif /* BSP_USING_TIM_TMRA_11 */

#ifdef BSP_USING_TIM_TMRA_12
#ifndef TIM_TMRA_12_CONFIG
#define TIM_TMRA_12_CONFIG                                      \
    {                                                           \
        .name           = "tim_a12",                            \
        .instance       = CM_TMRA_12,                           \
        .irq_config     =                                       \
        {                                                       \
            .irq_num    = TIM_TMRA_12_FLOW_IRQ_NUM,             \
            .irq_prio   = TIM_TMRA_12_FLOW_IRQ_PRIO,            \
            .int_src    = INT_SRC_TMRA_12_OVF,                  \
        },                                                      \
    }
#endif /* TIM_TMRA_12_CONFIG */
#endif /* BSP_USING_TIM_TMRA_12 */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_CONFIG_H__ */
