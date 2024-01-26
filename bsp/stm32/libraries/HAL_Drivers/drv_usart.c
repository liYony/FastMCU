/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023-2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-01     KyleChan     first version
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#include "board.h"
#include "drv_usart.h"

#ifdef FM_USING_SERIAL

#define DBG_TAG              "drv.usart"
#define DBG_LVL               DBG_INFO
#include <fmdbg.h>

#if !defined(BSP_USING_UART1) && !defined(BSP_USING_UART2) && !defined(BSP_USING_UART3) && \
    !defined(BSP_USING_UART4) && !defined(BSP_USING_UART5) && !defined(BSP_USING_UART6) && \
    !defined(BSP_USING_UART7) && !defined(BSP_USING_UART8) && !defined(BSP_USING_LPUART1)
    #error "Please define at least one BSP_USING_UARTx"
    /* this driver can be disabled at menuconfig -> RT-Thread Components -> Device Drivers */
#endif

#ifdef FM_SERIAL_USING_DMA
static void stm32_dma_config(struct fm_serial_device *serial, fm_ubase_t flag);
#endif

enum
{
#ifdef BSP_USING_UART1
    UART1_INDEX,
#endif

#ifdef BSP_USING_UART2
    UART2_INDEX,
#endif

#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif

#ifdef BSP_USING_UART4
    UART4_INDEX,
#endif

#ifdef BSP_USING_UART5
    UART5_INDEX,
#endif

#ifdef BSP_USING_UART6
    UART6_INDEX,
#endif

#ifdef BSP_USING_UART7
    UART7_INDEX,
#endif

#ifdef BSP_USING_UART8
    UART8_INDEX,
#endif

#ifdef BSP_USING_LPUART1
    LPUART1_INDEX,
#endif
};

static struct stm32_uart_config uart_config[] =
{
#ifdef BSP_USING_UART1
    UART1_CONFIG,
#endif

#ifdef BSP_USING_UART2
    UART2_CONFIG,
#endif

#ifdef BSP_USING_UART3
    UART3_CONFIG,
#endif

#ifdef BSP_USING_UART4
    UART4_CONFIG,
#endif
#ifdef BSP_USING_UART5
    UART5_CONFIG,
#endif
#ifdef BSP_USING_UART6
    UART6_CONFIG,
#endif
#ifdef BSP_USING_UART7
    UART7_CONFIG,
#endif
#ifdef BSP_USING_UART8
    UART8_CONFIG,
#endif
#ifdef BSP_USING_LPUART1
    LPUART1_CONFIG,
#endif
};


static struct stm32_uart uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

static fm_err_t stm32_configure(struct fm_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart *uart;
    FM_ASSERT(serial != FM_NULL);
    FM_ASSERT(cfg != FM_NULL);

    uart = fm_container_of(serial, struct stm32_uart, serial);
    uart->handle.Instance          = uart->config->Instance;
    uart->handle.Init.BaudRate     = cfg->baud_rate;
    uart->handle.Init.Mode         = UART_MODE_TX_RX;
#ifdef USART_CR1_OVER8
    uart->handle.Init.OverSampling = cfg->baud_rate > 5000000 ? UART_OVERSAMPLING_8 : UART_OVERSAMPLING_16;
#else
    uart->handle.Init.OverSampling = UART_OVERSAMPLING_16;
#endif /* USART_CR1_OVER8 */

    switch (cfg->data_bits)
    {
    case DATA_BITS_8:
        if (cfg->parity == PARITY_ODD || cfg->parity == PARITY_EVEN)
            uart->handle.Init.WordLength = UART_WORDLENGTH_9B;
        else
            uart->handle.Init.WordLength = UART_WORDLENGTH_8B;
        break;
    case DATA_BITS_9:
        uart->handle.Init.WordLength = UART_WORDLENGTH_9B;
        break;
    default:
        uart->handle.Init.WordLength = UART_WORDLENGTH_8B;
        break;
    }

    switch (cfg->stop_bits)
    {
    case STOP_BITS_1:
        uart->handle.Init.StopBits   = UART_STOPBITS_1;
        break;
    case STOP_BITS_2:
        uart->handle.Init.StopBits   = UART_STOPBITS_2;
        break;
    default:
        uart->handle.Init.StopBits   = UART_STOPBITS_1;
        break;
    }

    switch (cfg->parity)
    {
    case PARITY_NONE:
        uart->handle.Init.Parity     = UART_PARITY_NONE;
        break;
    case PARITY_ODD:
        uart->handle.Init.Parity     = UART_PARITY_ODD;
        break;
    case PARITY_EVEN:
        uart->handle.Init.Parity     = UART_PARITY_EVEN;
        break;
    default:
        uart->handle.Init.Parity     = UART_PARITY_NONE;
        break;
    }

    switch (cfg->flowcontrol)
    {
        case FM_SERIAL_FLOWCONTROL_NONE:
            uart->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            break;
        case FM_SERIAL_FLOWCONTROL_CTSRTS:
            uart->handle.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
            break;
        default:
            uart->handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            break;
    }

#ifdef FM_SERIAL_USING_DMA
    uart->dma_rx.remaining_cnt = serial->config.rx_bufsz;
#endif

    if (HAL_UART_Init(&uart->handle) != HAL_OK)
    {
        return -FM_ERROR;
    }

    return FM_EOK;
}

static fm_err_t stm32_control(struct fm_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart *uart;

    fm_ubase_t ctrl_arg = (fm_ubase_t)arg;

    FM_ASSERT(serial != FM_NULL);
    uart = fm_container_of(serial, struct stm32_uart, serial);

    if(ctrl_arg & (FM_DEVICE_FLAG_RX_BLOCKING | FM_DEVICE_FLAG_RX_NON_BLOCKING))
    {
        if (uart->uart_dma_flag & FM_DEVICE_FLAG_DMA_RX)
            ctrl_arg = FM_DEVICE_FLAG_DMA_RX;
        else
            ctrl_arg = FM_DEVICE_FLAG_INT_RX;
    }
    else if(ctrl_arg & (FM_DEVICE_FLAG_TX_BLOCKING | FM_DEVICE_FLAG_TX_NON_BLOCKING))
    {
        if (uart->uart_dma_flag & FM_DEVICE_FLAG_DMA_TX)
            ctrl_arg = FM_DEVICE_FLAG_DMA_TX;
        else
            ctrl_arg = FM_DEVICE_FLAG_INT_TX;
    }

    switch (cmd)
    {
    /* disable interrupt */
    case FM_DEVICE_CTRL_CLR_INT:

        NVIC_DisableIRQ(uart->config->irq_type);
        if (ctrl_arg == FM_DEVICE_FLAG_INT_RX)
            __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_RXNE);
        else if (ctrl_arg == FM_DEVICE_FLAG_INT_TX)
            __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_TXE);
#ifdef FM_SERIAL_USING_DMA
        else if (ctrl_arg == FM_DEVICE_FLAG_DMA_RX)
        {
            __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_RXNE);

            HAL_NVIC_DisableIRQ(uart->config->dma_rx->dma_irq);
            if (HAL_DMA_Abort(&(uart->dma_rx.handle)) != HAL_OK)
            {
                FM_ASSERT(0);
            }

            if (HAL_DMA_DeInit(&(uart->dma_rx.handle)) != HAL_OK)
            {
                FM_ASSERT(0);
            }
        }
        else if(ctrl_arg == FM_DEVICE_FLAG_DMA_TX)
        {
            __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_TC);

            HAL_NVIC_DisableIRQ(uart->config->dma_tx->dma_irq);
            if (HAL_DMA_DeInit(&(uart->dma_tx.handle)) != HAL_OK)
            {
                FM_ASSERT(0);
            }
        }
#endif
        break;

    case FM_DEVICE_CTRL_SET_INT:

        HAL_NVIC_SetPriority(uart->config->irq_type, 1, 0);
        HAL_NVIC_EnableIRQ(uart->config->irq_type);

        if (ctrl_arg == FM_DEVICE_FLAG_INT_RX)
            __HAL_UART_ENABLE_IT(&(uart->handle), UART_IT_RXNE);
        else if (ctrl_arg == FM_DEVICE_FLAG_INT_TX)
            __HAL_UART_ENABLE_IT(&(uart->handle), UART_IT_TXE);
        break;

    case FM_DEVICE_CTRL_CONFIG:
        if (ctrl_arg & (FM_DEVICE_FLAG_DMA_RX | FM_DEVICE_FLAG_DMA_TX))
        {

#ifdef FM_SERIAL_USING_DMA
            stm32_dma_config(serial, ctrl_arg);
#endif
        }
        else
            stm32_control(serial, FM_DEVICE_CTRL_SET_INT, (void *)ctrl_arg);
        break;

    case FM_DEVICE_CHECK_OPTMODE:
        {
            if (ctrl_arg & FM_DEVICE_FLAG_DMA_TX)
                return FM_SERIAL_TX_BLOCKING_NO_BUFFER;
            else
                return FM_SERIAL_TX_BLOCKING_BUFFER;
        }
    case FM_DEVICE_CTRL_CLOSE:
        if (HAL_UART_DeInit(&(uart->handle)) != HAL_OK )
        {
            FM_ASSERT(0)
        }
        break;

    }
    return FM_EOK;
}

static int stm32_putc(struct fm_serial_device *serial, char c)
{
    struct stm32_uart *uart;
    FM_ASSERT(serial != FM_NULL);

    uart = fm_container_of(serial, struct stm32_uart, serial);
    UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TC);
    UART_SET_TDR(&uart->handle, c);
    while (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TC) == RESET);

    return 1;
}

fm_uint32_t stm32_uart_get_mask(fm_uint32_t word_length, fm_uint32_t parity)
{
    fm_uint32_t mask = 0;
    if (word_length == UART_WORDLENGTH_8B)
    {
        if (parity == UART_PARITY_NONE)
        {
            mask = 0x00FFU ;
        }
        else
        {
            mask = 0x007FU ;
        }
    }
#ifdef UART_WORDLENGTH_9B
    else if (word_length == UART_WORDLENGTH_9B)
    {
        if (parity == UART_PARITY_NONE)
        {
            mask = 0x01FFU ;
        }
        else
        {
            mask = 0x00FFU ;
        }
    }
#endif
#ifdef UART_WORDLENGTH_7B
    else if (word_length == UART_WORDLENGTH_7B)
    {
        if (parity == UART_PARITY_NONE)
        {
            mask = 0x007FU ;
        }
        else
        {
            mask = 0x003FU ;
        }
    }
    else
    {
        mask = 0x0000U;
    }
#endif
    return mask;
}

static int stm32_getc(struct fm_serial_device *serial)
{
    int ch;
    struct stm32_uart *uart;
    FM_ASSERT(serial != FM_NULL);
    uart = fm_container_of(serial, struct stm32_uart, serial);

    ch = -1;
    if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_RXNE) != RESET)
        ch = UART_GET_RDR(&uart->handle, stm32_uart_get_mask(uart->handle.Init.WordLength, uart->handle.Init.Parity));
    return ch;
}

static fm_ssize_t stm32_transmit(struct fm_serial_device     *serial,
                                       fm_uint8_t           *buf,
                                       fm_size_t             size,
                                       fm_uint32_t           tx_flag)
{
    struct stm32_uart *uart;

    FM_ASSERT(serial != FM_NULL);
    FM_ASSERT(buf != FM_NULL);
    uart = fm_container_of(serial, struct stm32_uart, serial);

    if (uart->uart_dma_flag & FM_DEVICE_FLAG_DMA_TX)
    {
        HAL_UART_Transmit_DMA(&uart->handle, buf, size);
        return size;
    }

    stm32_control(serial, FM_DEVICE_CTRL_SET_INT, (void *)tx_flag);

    return size;
}

#ifdef FM_SERIAL_USING_DMA
static void dma_recv_isr(struct fm_serial_device *serial, fm_uint8_t isr_flag)
{
    struct stm32_uart *uart;
    fm_size_t recv_len, counter;

    FM_ASSERT(serial != FM_NULL);
    uart = fm_container_of(serial, struct stm32_uart, serial);

    recv_len = 0;
    counter = __HAL_DMA_GET_COUNTER(&(uart->dma_rx.handle));

    if (counter <= uart->dma_rx.remaining_cnt)
        recv_len = uart->dma_rx.remaining_cnt - counter;
    else
        recv_len = serial->config.rx_bufsz + uart->dma_rx.remaining_cnt - counter;
    if (recv_len)
    {
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
        struct fm_serial_rx_fifo *rx_fifo = (struct fm_serial_rx_fifo *) serial->serial_rx;
        SCB_InvalidateDCache_by_Addr((uint32_t *)rx_fifo->buffer, serial->config.rx_bufsz);
#endif
        uart->dma_rx.remaining_cnt = counter;
        fm_hw_serial_isr(serial, FM_SERIAL_EVENT_RX_DMADONE | (recv_len << 8));
    }
}
#endif  /* FM_SERIAL_USING_DMA */

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(struct fm_serial_device *serial)
{
    struct stm32_uart *uart;

    FM_ASSERT(serial != FM_NULL);
    uart = fm_container_of(serial, struct stm32_uart, serial);
    /* If the Read data register is not empty and the RXNE interrupt is enabled  (RDR) */
    if ((__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_RXNE) != RESET) &&
            (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_RXNE) != RESET))
    {
        struct fm_serial_rx_fifo *rx_fifo;
        rx_fifo = (struct fm_serial_rx_fifo *) serial->serial_rx;
        FM_ASSERT(rx_fifo != FM_NULL);

        fm_ringbuffer_putchar(&(rx_fifo->rb), UART_GET_RDR(&uart->handle, stm32_uart_get_mask(uart->handle.Init.WordLength, uart->handle.Init.Parity)));

        fm_hw_serial_isr(serial, FM_SERIAL_EVENT_RX_IND);
    }
    /* If the Transmit data register is empty and the TXE interrupt enable is enabled  (TDR) */
    else if ((__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TXE) != RESET) &&
                (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_TXE)) != RESET)
    {
        struct fm_serial_tx_fifo *tx_fifo;
        tx_fifo = (struct fm_serial_tx_fifo *) serial->serial_tx;
        FM_ASSERT(tx_fifo != FM_NULL);

        fm_uint8_t put_char = 0;
        if (fm_ringbuffer_getchar(&(tx_fifo->rb), &put_char))
        {
            UART_SET_TDR(&uart->handle, put_char);
        }
        else
        {
            __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_TXE);
            __HAL_UART_ENABLE_IT(&(uart->handle), UART_IT_TC);
        }
    }
    else if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TC) &&
            (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_TC) != RESET))
    {
        if (uart->uart_dma_flag & FM_DEVICE_FLAG_DMA_TX)
        {
            /* The HAL_UART_TxCpltCallback will be triggered */
            HAL_UART_IRQHandler(&(uart->handle));
        }
        else
        {
            /* Transmission complete interrupt disable ( CR1 Register) */
            __HAL_UART_DISABLE_IT(&(uart->handle), UART_IT_TC);
            fm_hw_serial_isr(serial, FM_SERIAL_EVENT_TX_DONE);
        }
        /* Clear Transmission complete interrupt flag ( ISR Register ) */
        UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TC);
    }

#ifdef FM_SERIAL_USING_DMA
    else if ((uart->uart_dma_flag) && (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_IDLE) != RESET)
             && (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_IDLE) != RESET))
    {
        dma_recv_isr(serial, UART_RX_DMA_IT_IDLE_FLAG);
        __HAL_UART_CLEAR_IDLEFLAG(&uart->handle);
    }
#endif
    else
    {
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_ORE) != RESET)
        {
            LOG_E("(%s) serial device Overrun error!", serial->parent.parent.name);
            __HAL_UART_CLEAR_OREFLAG(&uart->handle);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_NE) != RESET)
        {
            __HAL_UART_CLEAR_NEFLAG(&uart->handle);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_FE) != RESET)
        {
            __HAL_UART_CLEAR_FEFLAG(&uart->handle);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_PE) != RESET)
        {
            __HAL_UART_CLEAR_PEFLAG(&uart->handle);
        }
#if !defined(SOC_SERIES_STM32L4) && !defined(SOC_SERIES_STM32WL) && !defined(SOC_SERIES_STM32F7) && !defined(SOC_SERIES_STM32F0) \
    && !defined(SOC_SERIES_STM32L0) && !defined(SOC_SERIES_STM32G0) && !defined(SOC_SERIES_STM32H7) \
    && !defined(SOC_SERIES_STM32G4) && !defined(SOC_SERIES_STM32MP1) && !defined(SOC_SERIES_STM32WB)
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_LBD) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_LBD);
        }
#endif
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_CTS) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_CTS);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TXE) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TXE);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_TC) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_TC);
        }
        if (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_RXNE) != RESET)
        {
            UART_INSTANCE_CLEAR_FUNCTION(&(uart->handle), UART_FLAG_RXNE);
        }
    }
}

#if defined(BSP_USING_UART1)
void USART1_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART1_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART1_RX_USING_DMA)
void UART1_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART1_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART1_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART1_TX_USING_DMA)
void UART1_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART1_INDEX].dma_tx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART1_TX_USING_DMA) */
#endif /* BSP_USING_UART1 */

#if defined(BSP_USING_UART2)
void USART2_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART2_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART2_RX_USING_DMA)
void UART2_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART2_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART2_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART2_TX_USING_DMA)
void UART2_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART2_INDEX].dma_tx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART2_TX_USING_DMA) */
#endif /* BSP_USING_UART2 */

#if defined(BSP_USING_UART3)
void USART3_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART3_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART3_RX_USING_DMA)
void UART3_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART3_INDEX].dma_rx.handle);
}
#endif /* defined(BSP_UART_USING_DMA_RX) && defined(BSP_UART3_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART3_TX_USING_DMA)
void UART3_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART3_INDEX].dma_tx.handle);
}
#endif /* defined(BSP_UART_USING_DMA_TX) && defined(BSP_UART3_TX_USING_DMA) */
#endif /* BSP_USING_UART3*/

#if defined(BSP_USING_UART4)
void UART4_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART4_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART4_RX_USING_DMA)
void UART4_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART4_INDEX].dma_rx.handle);
}
#endif /* defined(BSP_UART_USING_DMA_RX) && defined(BSP_UART4_RX_USING_DMA) */

#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART4_TX_USING_DMA)
void UART4_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART4_INDEX].dma_tx.handle);
}
#endif /* defined(BSP_UART_USING_DMA_TX) && defined(BSP_UART4_TX_USING_DMA) */
#endif /* BSP_USING_UART4*/

#if defined(BSP_USING_UART5)
void UART5_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART5_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART5_RX_USING_DMA)
void UART5_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART5_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART5_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART5_TX_USING_DMA)
void UART5_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART5_INDEX].dma_tx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART5_TX_USING_DMA) */
#endif /* BSP_USING_UART5*/

#if defined(BSP_USING_UART6)
void USART6_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART6_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART6_RX_USING_DMA)
void UART6_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART6_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART6_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART6_TX_USING_DMA)
void UART6_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART6_INDEX].dma_tx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART6_TX_USING_DMA) */
#endif /* BSP_USING_UART6*/

#if defined(BSP_USING_UART7)
void UART7_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART7_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART7_RX_USING_DMA)
void UART7_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART7_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART7_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART7_TX_USING_DMA)
void UART7_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART7_INDEX].dma_tx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART7_TX_USING_DMA) */
#endif /* BSP_USING_UART7*/

#if defined(BSP_USING_UART8)
void UART8_IRQHandler(void)
{
    uart_isr(&(uart_obj[UART8_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART8_RX_USING_DMA)
void UART8_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART8_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART8_RX_USING_DMA) */
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_UART8_TX_USING_DMA)
void UART8_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[UART8_INDEX].dma_tx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_UART8_TX_USING_DMA) */
#endif /* BSP_USING_UART8*/

#if defined(BSP_USING_LPUART1)
void LPUART1_IRQHandler(void)
{
    uart_isr(&(uart_obj[LPUART1_INDEX].serial));
}
#if defined(FM_SERIAL_USING_DMA) && defined(BSP_LPUART1_RX_USING_DMA)
void LPUART1_DMA_RX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&uart_obj[LPUART1_INDEX].dma_rx.handle);
}
#endif /* defined(FM_SERIAL_USING_DMA) && defined(BSP_LPUART1_RX_USING_DMA) */
#endif /* BSP_USING_LPUART1*/
static void stm32_uart_get_config(void)
{
    struct serial_configure config = FM_SERIAL_CONFIG_DEFAULT;
#ifdef BSP_USING_UART1
    uart_obj[UART1_INDEX].serial.config = config;
    uart_obj[UART1_INDEX].uart_dma_flag = 0;

    uart_obj[UART1_INDEX].serial.config.rx_bufsz = BSP_UART1_RX_BUFSIZE;
    uart_obj[UART1_INDEX].serial.config.tx_bufsz = BSP_UART1_TX_BUFSIZE;

#ifdef BSP_UART1_RX_USING_DMA
    uart_obj[UART1_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart1_dma_rx = UART1_DMA_RX_CONFIG;
    uart_config[UART1_INDEX].dma_rx = &uart1_dma_rx;
#endif

#ifdef BSP_UART1_TX_USING_DMA
    uart_obj[UART1_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart1_dma_tx = UART1_DMA_TX_CONFIG;
    uart_config[UART1_INDEX].dma_tx = &uart1_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART2
    uart_obj[UART2_INDEX].serial.config = config;
    uart_obj[UART2_INDEX].uart_dma_flag = 0;

    uart_obj[UART2_INDEX].serial.config.rx_bufsz = BSP_UART2_RX_BUFSIZE;
    uart_obj[UART2_INDEX].serial.config.tx_bufsz = BSP_UART2_TX_BUFSIZE;

#ifdef BSP_UART2_RX_USING_DMA
    uart_obj[UART2_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart2_dma_rx = UART2_DMA_RX_CONFIG;
    uart_config[UART2_INDEX].dma_rx = &uart2_dma_rx;
#endif

#ifdef BSP_UART2_TX_USING_DMA
    uart_obj[UART2_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart2_dma_tx = UART2_DMA_TX_CONFIG;
    uart_config[UART2_INDEX].dma_tx = &uart2_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART3
    uart_obj[UART3_INDEX].serial.config = config;
    uart_obj[UART3_INDEX].uart_dma_flag = 0;

    uart_obj[UART3_INDEX].serial.config.rx_bufsz = BSP_UART3_RX_BUFSIZE;
    uart_obj[UART3_INDEX].serial.config.tx_bufsz = BSP_UART3_TX_BUFSIZE;

#ifdef BSP_UART3_RX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart3_dma_rx = UART3_DMA_RX_CONFIG;
    uart_config[UART3_INDEX].dma_rx = &uart3_dma_rx;
#endif

#ifdef BSP_UART3_TX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart3_dma_tx = UART3_DMA_TX_CONFIG;
    uart_config[UART3_INDEX].dma_tx = &uart3_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART4
    uart_obj[UART4_INDEX].serial.config = config;
    uart_obj[UART4_INDEX].uart_dma_flag = 0;

    uart_obj[UART4_INDEX].serial.config.rx_bufsz = BSP_UART4_RX_BUFSIZE;
    uart_obj[UART4_INDEX].serial.config.tx_bufsz = BSP_UART4_TX_BUFSIZE;

#ifdef BSP_UART4_RX_USING_DMA
    uart_obj[UART4_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart4_dma_rx = UART4_DMA_RX_CONFIG;
    uart_config[UART4_INDEX].dma_rx = &uart4_dma_rx;
#endif

#ifdef BSP_UART4_TX_USING_DMA
    uart_obj[UART4_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart4_dma_tx = UART4_DMA_TX_CONFIG;
    uart_config[UART4_INDEX].dma_tx = &uart4_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART5
    uart_obj[UART5_INDEX].serial.config = config;
    uart_obj[UART5_INDEX].uart_dma_flag = 0;

    uart_obj[UART5_INDEX].serial.config.rx_bufsz = BSP_UART5_RX_BUFSIZE;
    uart_obj[UART5_INDEX].serial.config.tx_bufsz = BSP_UART5_TX_BUFSIZE;

#ifdef BSP_UART5_RX_USING_DMA
    uart_obj[UART5_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart5_dma_rx = UART5_DMA_RX_CONFIG;
    uart_config[UART5_INDEX].dma_rx = &uart5_dma_rx;
#endif

#ifdef BSP_UART5_TX_USING_DMA
    uart_obj[UART5_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart5_dma_tx = UART5_DMA_TX_CONFIG;
    uart_config[UART5_INDEX].dma_tx = &uart5_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART6
    uart_obj[UART6_INDEX].serial.config = config;
    uart_obj[UART6_INDEX].uart_dma_flag = 0;

    uart_obj[UART6_INDEX].serial.config.rx_bufsz = BSP_UART6_RX_BUFSIZE;
    uart_obj[UART6_INDEX].serial.config.tx_bufsz = BSP_UART6_TX_BUFSIZE;

#ifdef BSP_UART6_RX_USING_DMA
    uart_obj[UART6_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart6_dma_rx = UART6_DMA_RX_CONFIG;
    uart_config[UART6_INDEX].dma_rx = &uart6_dma_rx;
#endif

#ifdef BSP_UART6_TX_USING_DMA
    uart_obj[UART6_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart6_dma_tx = UART6_DMA_TX_CONFIG;
    uart_config[UART6_INDEX].dma_tx = &uart6_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART7
    uart_obj[UART7_INDEX].serial.config = config;
    uart_obj[UART7_INDEX].uart_dma_flag = 0;

    uart_obj[UART7_INDEX].serial.config.rx_bufsz = BSP_UART7_RX_BUFSIZE;
    uart_obj[UART7_INDEX].serial.config.tx_bufsz = BSP_UART7_TX_BUFSIZE;

#ifdef BSP_UART7_RX_USING_DMA
    uart_obj[UART7_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart7_dma_rx = UART7_DMA_RX_CONFIG;
    uart_config[UART7_INDEX].dma_rx = &uart7_dma_rx;
#endif

#ifdef BSP_UART7_TX_USING_DMA
    uart_obj[UART7_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart7_dma_tx = UART7_DMA_TX_CONFIG;
    uart_config[UART7_INDEX].dma_tx = &uart7_dma_tx;
#endif
#endif

#ifdef BSP_USING_UART8
    uart_obj[UART8_INDEX].serial.config = config;
    uart_obj[UART8_INDEX].uart_dma_flag = 0;

    uart_obj[UART8_INDEX].serial.config.rx_bufsz = BSP_UART8_RX_BUFSIZE;
    uart_obj[UART8_INDEX].serial.config.tx_bufsz = BSP_UART8_TX_BUFSIZE;

#ifdef BSP_UART8_RX_USING_DMA
    uart_obj[UART8_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart8_dma_rx = UART8_DMA_RX_CONFIG;
    uart_config[UART8_INDEX].dma_rx = &uart8_dma_rx;
#endif

#ifdef BSP_UART8_TX_USING_DMA
    uart_obj[UART8_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart8_dma_tx = UART8_DMA_TX_CONFIG;
    uart_config[UART8_INDEX].dma_tx = &uart8_dma_tx;
#endif
#endif
}

#ifdef FM_SERIAL_USING_DMA
static void stm32_dma_config(struct fm_serial_device *serial, fm_ubase_t flag)
{
    struct fm_serial_rx_fifo *rx_fifo;
    DMA_HandleTypeDef *DMA_Handle;
    struct dma_config *dma_config;
    struct stm32_uart *uart;

    FM_ASSERT(serial != FM_NULL);
    FM_ASSERT(flag == FM_DEVICE_FLAG_DMA_TX || flag == FM_DEVICE_FLAG_DMA_RX);
    uart = fm_container_of(serial, struct stm32_uart, serial);

    if (FM_DEVICE_FLAG_DMA_RX == flag)
    {
        DMA_Handle = &uart->dma_rx.handle;
        dma_config = uart->config->dma_rx;
    }
    else /* FM_DEVICE_FLAG_DMA_TX == flag */
    {
        DMA_Handle = &uart->dma_tx.handle;
        dma_config = uart->config->dma_tx;
    }
    LOG_D("%s dma config start", uart->config->name);

    {
        fm_uint32_t tmpreg = 0x00U;
#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32G0) \
    || defined(SOC_SERIES_STM32L0)
        /* enable DMA clock && Delay after an RCC peripheral clock enabling*/
        SET_BIT(RCC->AHBENR, dma_config->dma_rcc);
        tmpreg = READ_BIT(RCC->AHBENR, dma_config->dma_rcc);
#elif defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) \
    || defined(SOC_SERIES_STM32G4)|| defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32WB)
        /* enable DMA clock && Delay after an RCC peripheral clock enabling*/
        SET_BIT(RCC->AHB1ENR, dma_config->dma_rcc);
        tmpreg = READ_BIT(RCC->AHB1ENR, dma_config->dma_rcc);
#elif defined(SOC_SERIES_STM32MP1)
        /* enable DMA clock && Delay after an RCC peripheral clock enabling*/
        SET_BIT(RCC->MP_AHB2ENSETR, dma_config->dma_rcc);
        tmpreg = READ_BIT(RCC->MP_AHB2ENSETR, dma_config->dma_rcc);
#endif

#if defined(DMAMUX1) && (defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB))
        /* enable DMAMUX clock for L4+ and G4 */
        __HAL_RCC_DMAMUX1_CLK_ENABLE();
#elif defined(SOC_SERIES_STM32MP1)
        __HAL_RCC_DMAMUX_CLK_ENABLE();
#endif

        UNUSED(tmpreg);   /* To avoid compiler warnings */
    }

    if (FM_DEVICE_FLAG_DMA_RX == flag)
    {
        __HAL_LINKDMA(&(uart->handle), hdmarx, uart->dma_rx.handle);
    }
    else if (FM_DEVICE_FLAG_DMA_TX == flag)
    {
        __HAL_LINKDMA(&(uart->handle), hdmatx, uart->dma_tx.handle);
    }

#if defined(SOC_SERIES_STM32F1) || defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32L0)
    DMA_Handle->Instance                 = dma_config->Instance;
#elif defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7)
    DMA_Handle->Instance                 = dma_config->Instance;
    DMA_Handle->Init.Channel             = dma_config->channel;
#elif defined(SOC_SERIES_STM32L4) || defined(SOC_SERIES_STM32WL) || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32WB)\
    || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1)
    DMA_Handle->Instance                 = dma_config->Instance;
    DMA_Handle->Init.Request             = dma_config->request;
#endif
    DMA_Handle->Init.PeriphInc           = DMA_PINC_DISABLE;
    DMA_Handle->Init.MemInc              = DMA_MINC_ENABLE;
    DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    DMA_Handle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;

    if (FM_DEVICE_FLAG_DMA_RX == flag)
    {
        DMA_Handle->Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DMA_Handle->Init.Mode                = DMA_CIRCULAR;
    }
    else if (FM_DEVICE_FLAG_DMA_TX == flag)
    {
        DMA_Handle->Init.Direction           = DMA_MEMORY_TO_PERIPH;
        DMA_Handle->Init.Mode                = DMA_NORMAL;
    }

    DMA_Handle->Init.Priority            = DMA_PRIORITY_MEDIUM;
#if defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32H7) || defined(SOC_SERIES_STM32MP1)
    DMA_Handle->Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
#endif
    if (HAL_DMA_DeInit(DMA_Handle) != HAL_OK)
    {
        FM_ASSERT(0);
    }

    if (HAL_DMA_Init(DMA_Handle) != HAL_OK)
    {
        FM_ASSERT(0);
    }

    /* enable interrupt */
    if (flag == FM_DEVICE_FLAG_DMA_RX)
    {
        rx_fifo = (struct fm_serial_rx_fifo *)serial->serial_rx;
        FM_ASSERT(rx_fifo != FM_NULL);
        /* Start DMA transfer */
        if (HAL_UART_Receive_DMA(&(uart->handle), rx_fifo->buffer, serial->config.rx_bufsz) != HAL_OK)
        {
            /* Transfer error in reception process */
            FM_ASSERT(0);
        }
        CLEAR_BIT(uart->handle.Instance->CR3, USART_CR3_EIE);
        __HAL_UART_ENABLE_IT(&(uart->handle), UART_IT_IDLE);
    }

    /* DMA irq should set in DMA TX mode, or HAL_UART_TxCpltCallback function will not be called */
    HAL_NVIC_SetPriority(dma_config->dma_irq, 0, 0);
    HAL_NVIC_EnableIRQ(dma_config->dma_irq);

    HAL_NVIC_SetPriority(uart->config->irq_type, 1, 0);
    HAL_NVIC_EnableIRQ(uart->config->irq_type);

    LOG_D("%s dma %s instance: %x", uart->config->name, flag == FM_DEVICE_FLAG_DMA_RX ? "RX" : "TX", DMA_Handle->Instance);
    LOG_D("%s dma config done", uart->config->name);
}

/**
  * @brief  UART error callbacks
  * @param  huart: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    FM_ASSERT(huart != NULL);
    struct stm32_uart *uart = (struct stm32_uart *)huart;
    LOG_D("%s: %s %d\n", __FUNCTION__, uart->config->name, huart->ErrorCode);
    UNUSED(uart);
}

/**
  * @brief  Rx Transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    struct stm32_uart *uart;
    FM_ASSERT(huart != NULL);
    uart = (struct stm32_uart *)huart;
    dma_recv_isr(&uart->serial, UART_RX_DMA_IT_TC_FLAG);
}

/**
  * @brief  Rx Half transfer completed callback
  * @param  huart: UART handle
  * @note   This example shows a simple way to report end of DMA Rx Half transfer,
  *         and you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    struct stm32_uart *uart;
    FM_ASSERT(huart != NULL);
    uart = (struct stm32_uart *)huart;
    dma_recv_isr(&uart->serial, UART_RX_DMA_IT_HT_FLAG);
}

/**
  * @brief  HAL_UART_TxCpltCallback
  * @param  huart: UART handle
  * @note   This callback can be called by two functions, first in UART_EndTransmit_IT when
  *         UART Tx complete and second in UART_DMATransmitCplt function in DMA Circular mode.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    struct stm32_uart *uart;
    struct fm_serial_device *serial;
    fm_size_t trans_total_index;
    fm_base_t level;

    FM_ASSERT(huart != NULL);
    uart = (struct stm32_uart *)huart;
    serial = &uart->serial;
    FM_ASSERT(serial != FM_NULL);

    level = fm_hw_interrupt_disable();
    trans_total_index = __HAL_DMA_GET_COUNTER(&(uart->dma_tx.handle));
    fm_hw_interrupt_enable(level);

    if (trans_total_index) return;

    fm_hw_serial_isr(serial, FM_SERIAL_EVENT_TX_DMADONE);

}
#endif  /* FM_SERIAL_USING_DMA */

static const struct fm_serial_ops stm32_uart_ops =
{
    .configure = stm32_configure,
    .control = stm32_control,
    .putc = stm32_putc,
    .getc = stm32_getc,
    .transmit = stm32_transmit
};

int fm_hw_usart_init(void)
{
    fm_err_t result = 0;
    fm_size_t obj_num = sizeof(uart_obj) / sizeof(struct stm32_uart);

    stm32_uart_get_config();
    for (int i = 0; i < obj_num; i++)
    {
        /* init UART object */
        uart_obj[i].config = &uart_config[i];
        uart_obj[i].serial.ops = &stm32_uart_ops;
        /* register UART device */
        result = fm_hw_serial_register(&uart_obj[i].serial,
                                        uart_obj[i].config->name,
                                        FM_DEVICE_FLAG_RDWR,
                                        NULL);
        FM_ASSERT(result == FM_EOK);
    }

    return result;
}

#endif /* FM_USING_SERIAL */
