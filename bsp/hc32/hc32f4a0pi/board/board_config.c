/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 */

#include <fmdevice.h>
#include "board_config.h"

/**
 * The below functions will initialize HC32 board.
 */

#if defined FM_USING_SERIAL
fm_err_t fm_hw_board_uart_init(CM_USART_TypeDef *USARTx)
{
    fm_err_t result = FM_EOK;

    switch ((fm_uint32_t)USARTx)
    {
#if defined(BSP_USING_UART1)
    case (fm_uint32_t)CM_USART1:
        /* Configure USART RX/TX pin. */
        GPIO_SetFunc(USART1_RX_PORT, USART1_RX_PIN, GPIO_FUNC_33);
        GPIO_SetFunc(USART1_TX_PORT, USART1_TX_PIN, GPIO_FUNC_32);
        break;
#endif
#if defined(BSP_USING_UART3)
    case (fm_uint32_t)CM_USART3:
        /* Configure USART RX/TX pin. */
        GPIO_SetFunc(USART3_RX_PORT, USART3_RX_PIN, GPIO_FUNC_37);
        GPIO_SetFunc(USART3_TX_PORT, USART3_TX_PIN, GPIO_FUNC_36);
        break;
#endif
#if defined(BSP_USING_UART6)
    case (fm_uint32_t)CM_USART6:
        /* Configure USART RX/TX pin. */
        GPIO_SetFunc(USART6_RX_PORT, USART6_RX_PIN, GPIO_FUNC_37);
        GPIO_SetFunc(USART6_TX_PORT, USART6_TX_PIN, GPIO_FUNC_36);
        break;
#endif
    default:
        result = -FM_ERROR;
        break;
    }

    return result;
}
#endif
