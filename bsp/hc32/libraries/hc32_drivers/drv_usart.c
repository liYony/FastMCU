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

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "board.h"
#include "drv_usart.h"

#ifdef FM_USING_SERIAL

#if defined(BSP_USING_UART1) || defined(BSP_USING_UART2) || defined(BSP_USING_UART3) || \
    defined(BSP_USING_UART4) || defined(BSP_USING_UART5) || defined(BSP_USING_UART6) || \
    defined(BSP_USING_UART7) || defined(BSP_USING_UART8) || defined(BSP_USING_UART9) || \
    defined(BSP_USING_UART10)

#include "drv_usart.h"
#include "board_config.h"

#define UART_BAUDRATE_ERR_MAX           (0.025F)

#if defined (HC32F460)
    #define FCG_USART_CLK               FCG_Fcg1PeriphClockCmd

#elif defined (HC32F4A0)
    #define FCG_USART_CLK               FCG_Fcg3PeriphClockCmd
#endif

extern fm_err_t fm_hw_board_uart_init(CM_USART_TypeDef *USARTx);

enum
{
#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif
};

static struct hc32_uart_config uart_config[] =
{
#ifdef BSP_USING_UART3
    UART3_CONFIG,
#endif
};

static struct hc32_uart uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

static fm_err_t hc32_configure(struct fm_serial_device *serial, struct serial_configure *cfg)
{
    struct hc32_uart *uart;
    stc_usart_uart_init_t uart_init;

    FM_ASSERT(FM_NULL != cfg);
    FM_ASSERT(FM_NULL != serial);

    uart = fm_container_of(serial, struct hc32_uart, serial);
    USART_UART_StructInit(&uart_init);
    uart_init.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    uart_init.u32Baudrate = cfg->baud_rate;
    uart_init.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
#if defined (HC32F4A0)
    if ((CM_USART1 == uart->config->Instance) || (CM_USART2 == uart->config->Instance) || \
            (CM_USART6 == uart->config->Instance) || (CM_USART7 == uart->config->Instance))
#elif defined (HC32F460)
    if ((CM_USART1 == uart->config->Instance) || (CM_USART2 == uart->config->Instance) || \
            (CM_USART3 == uart->config->Instance) || (CM_USART4 == uart->config->Instance))
#endif
    {
        uart_init.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    }

    switch (cfg->data_bits)
    {
    case DATA_BITS_8:
        uart_init.u32DataWidth = USART_DATA_WIDTH_8BIT;
        break;
    case DATA_BITS_9:
        uart_init.u32DataWidth = USART_DATA_WIDTH_9BIT;
        break;
    default:
        uart_init.u32DataWidth = USART_DATA_WIDTH_8BIT;
        break;
    }

    switch (cfg->stop_bits)
    {
    case STOP_BITS_1:
        uart_init.u32StopBit = USART_STOPBIT_1BIT;
        break;
    case STOP_BITS_2:
        uart_init.u32StopBit = USART_STOPBIT_2BIT;
        break;
    default:
        uart_init.u32StopBit = USART_STOPBIT_1BIT;
        break;
    }

    switch (cfg->parity)
    {
    case PARITY_NONE:
        uart_init.u32Parity = USART_PARITY_NONE;
        break;
    case PARITY_EVEN:
        uart_init.u32Parity = USART_PARITY_EVEN;
        break;
    case PARITY_ODD:
        uart_init.u32Parity = USART_PARITY_ODD;
        break;
    default:
        uart_init.u32Parity = USART_PARITY_NONE;
        break;
    }

    if (BIT_ORDER_LSB == cfg->bit_order)
    {
        uart_init.u32FirstBit = USART_FIRST_BIT_LSB;
    }
    else
    {
        uart_init.u32FirstBit = USART_FIRST_BIT_MSB;
    }
#if defined (HC32F4A0)
    switch (cfg->flowcontrol)
    {
    case FM_SERIAL_FLOWCONTROL_NONE:
        uart_init.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
        break;
    case FM_SERIAL_FLOWCONTROL_CTSRTS:
        uart_init.u32HWFlowControl = USART_HW_FLOWCTRL_RTS_CTS;
        break;
    default:
        uart_init.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
        break;
    }
#endif

    /* Enable USART clock */
    FCG_USART_CLK(uart->config->clock, ENABLE);
    if (FM_EOK != fm_hw_board_uart_init(uart->config->Instance))
    {
        return -FM_ERROR;
    }
    /* Configure UART */
    uint32_t u32Div;
    float32_t f32Error;
    int32_t i32Ret = LL_ERR;
    USART_DeInit(uart->config->Instance);
    USART_UART_Init(uart->config->Instance, &uart_init, NULL);
    for (u32Div = 0UL; u32Div <= USART_CLK_DIV64; u32Div++)
    {
        USART_SetClockDiv(uart->config->Instance, u32Div);
        if ((LL_OK == USART_SetBaudrate(uart->config->Instance, uart_init.u32Baudrate, &f32Error)) &&
                ((-UART_BAUDRATE_ERR_MAX <= f32Error) && (f32Error <= UART_BAUDRATE_ERR_MAX)))
        {
            i32Ret = LL_OK;
            break;
        }
    }
    if (i32Ret != LL_OK)
    {
        return -FM_ERROR;
    }

    /* Enable completion interrupt */
    NVIC_EnableIRQ(uart->config->tx_cplt_irq.irq_config.irq_num);
    /* Enable error interrupt */
    NVIC_EnableIRQ(uart->config->rxerr_irq.irq_config.irq_num);
    USART_FuncCmd(uart->config->Instance, USART_TX | USART_RX | USART_INT_RX, ENABLE);

    return FM_EOK;
}

static fm_err_t hc32_control(struct fm_serial_device *serial, int cmd, void *arg)
{
    struct hc32_uart *uart;
    fm_ubase_t ctrl_arg = (fm_ubase_t)arg;

    FM_ASSERT(FM_NULL != serial);
    uart = fm_container_of(serial, struct hc32_uart, serial);
    FM_ASSERT(FM_NULL != uart->config->Instance);

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
    /* Disable interrupt */
    case FM_DEVICE_CTRL_CLR_INT:
        if (FM_DEVICE_FLAG_INT_RX == ctrl_arg)
        {
            NVIC_DisableIRQ(uart->config->rx_irq.irq_config.irq_num);
            USART_FuncCmd(uart->config->Instance, USART_INT_RX, DISABLE);
            INTC_IrqSignOut(uart->config->rx_irq.irq_config.irq_num);
        }
        else if (FM_DEVICE_FLAG_INT_TX == ctrl_arg)
        {
            NVIC_DisableIRQ(uart->config->tx_irq.irq_config.irq_num);
            USART_FuncCmd(uart->config->Instance, USART_INT_TX_EMPTY, DISABLE);
            INTC_IrqSignOut(uart->config->tx_irq.irq_config.irq_num);
        }
        break;
    /* Enable interrupt */
    case FM_DEVICE_CTRL_SET_INT:
        if (FM_DEVICE_FLAG_INT_RX == ctrl_arg)
        {
            hc32_install_irq_handler(&uart->config->rx_irq.irq_config, uart->config->rx_irq.irq_callback, FM_TRUE);
            USART_FuncCmd(uart->config->Instance, USART_INT_RX, ENABLE);
        }
        else
        {
            hc32_install_irq_handler(&uart->config->tx_irq.irq_config, uart->config->tx_irq.irq_callback, FM_TRUE);
            USART_FuncCmd(uart->config->Instance, USART_TX, DISABLE);
            USART_FuncCmd(uart->config->Instance, USART_TX | USART_INT_TX_EMPTY, ENABLE);
        }
        break;
    case FM_DEVICE_CTRL_CONFIG:
        if (ctrl_arg & (FM_DEVICE_FLAG_DMA_RX | FM_DEVICE_FLAG_DMA_TX))
        {

        }
        else
            hc32_control(serial, FM_DEVICE_CTRL_SET_INT, (void *)ctrl_arg);
        break;

    case FM_DEVICE_CHECK_OPTMODE:
        {
            if (ctrl_arg & FM_DEVICE_FLAG_DMA_TX)
                return FM_SERIAL_TX_BLOCKING_NO_BUFFER;
            else
                return FM_SERIAL_TX_BLOCKING_BUFFER;
        }
    case FM_DEVICE_CTRL_CLOSE:
        USART_DeInit(uart->config->Instance);
        break;
    }

    return FM_EOK;
}

static int hc32_putc(struct fm_serial_device *serial, char c)
{
    struct hc32_uart *uart;

    FM_ASSERT(FM_NULL != serial);
    uart = fm_container_of(serial, struct hc32_uart, serial);
    FM_ASSERT(FM_NULL != uart->config->Instance);

    if (serial->parent.open_flag & FM_DEVICE_FLAG_INT_TX)
    {
        if (USART_GetStatus(uart->config->Instance, USART_FLAG_TX_EMPTY) != SET)
        {
            return -1;
        }
    }
    else
    {
        /* Polling mode. */
        while (USART_GetStatus(uart->config->Instance, USART_FLAG_TX_EMPTY) != SET);
    }
    USART_WriteData(uart->config->Instance, c);

    return 1;
}

static int hc32_getc(struct fm_serial_device *serial)
{
    int ch = -1;
    struct hc32_uart *uart;

    FM_ASSERT(FM_NULL != serial);

    uart = fm_container_of(serial, struct hc32_uart, serial);
    FM_ASSERT(FM_NULL != uart->config->Instance);
    if (SET == USART_GetStatus(uart->config->Instance, USART_FLAG_RX_FULL))
    {
        ch = (fm_uint8_t)USART_ReadData(uart->config->Instance);
    }

    return ch;
}

static fm_ssize_t hc32_transmit(struct fm_serial_device     *serial,
                                       fm_uint8_t           *buf,
                                       fm_size_t             size,
                                       fm_uint32_t           tx_flag)
{
    struct hc32_uart *uart;

    FM_ASSERT(serial != FM_NULL);
    FM_ASSERT(buf != FM_NULL);
    uart = fm_container_of(serial, struct hc32_uart, serial);

    if (uart->uart_dma_flag & FM_DEVICE_FLAG_DMA_TX)
    {
//        HAL_UART_Transmit_DMA(&uart->handle, buf, size);
        return size;
    }

    hc32_control(serial, FM_DEVICE_CTRL_SET_INT, (void *)tx_flag);

    return size;
}

static void hc32_uart_rx_irq_handler(struct hc32_uart *uart)
{
    FM_ASSERT(FM_NULL != uart);

    struct fm_serial_rx_fifo *rx_fifo;
    rx_fifo = (struct fm_serial_rx_fifo *) uart->serial.serial_rx;
    FM_ASSERT(rx_fifo != FM_NULL);

    fm_ringbuffer_putchar(&(rx_fifo->rb), (fm_uint8_t)USART_ReadData(uart->config->Instance));

    fm_hw_serial_isr(&uart->serial, FM_SERIAL_EVENT_RX_IND);
}

static void hc32_uart_tx_irq_handler(struct hc32_uart *uart)
{
    FM_ASSERT(FM_NULL != uart);

    struct fm_serial_tx_fifo *tx_fifo;
    tx_fifo = (struct fm_serial_tx_fifo *) uart->serial.serial_tx;
    FM_ASSERT(tx_fifo != FM_NULL);

    fm_uint8_t put_char = 0;
    if (fm_ringbuffer_getchar(&(tx_fifo->rb), &put_char))
    {
        USART_WriteData(uart->config->Instance, put_char);
    }
    else
    {
        USART_FuncCmd(uart->config->Instance, USART_INT_TX_EMPTY, DISABLE);
        USART_FuncCmd(uart->config->Instance, USART_INT_TX_CPLT, ENABLE);
    }
}

static void hc32_uart_tx_cplt_irq_handler(struct hc32_uart *uart)
{
    /* Transmission complete interrupt disable ( CR1 Register) */
    USART_FuncCmd(uart->config->Instance, (USART_TX | USART_INT_TX_CPLT | USART_INT_TX_EMPTY), DISABLE);

    fm_hw_serial_isr(&uart->serial, FM_SERIAL_EVENT_TX_DONE);
}

static void hc32_uart_rxerr_irq_handler(struct hc32_uart *uart)
{
    FM_ASSERT(FM_NULL != uart);
    FM_ASSERT(FM_NULL != uart->config->Instance);

    if (SET == USART_GetStatus(uart->config->Instance, (USART_FLAG_OVERRUN | USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR)))
    {
        USART_ReadData(uart->config->Instance);
    }
    USART_ClearStatus(uart->config->Instance, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

#if defined(BSP_USING_UART3)
static void hc32_uart3_rx_irq_handler(void)
{
    hc32_uart_rx_irq_handler(&uart_obj[UART3_INDEX]);
}

static void hc32_uart3_tx_irq_handler(void)
{
    hc32_uart_tx_irq_handler(&uart_obj[UART3_INDEX]);
}

static void hc32_uart3_tx_cplt_irq_handler(void)
{
    hc32_uart_tx_cplt_irq_handler(&uart_obj[UART3_INDEX]);
}

static void hc32_uart3_rxerr_irq_handler(void)
{
    hc32_uart_rxerr_irq_handler(&uart_obj[UART3_INDEX]);
}
#endif
/**
  * @brief  This function gets dma witch uart used infomation include unit,
  *         channel, interrupt etc.
  * @param  None
  * @retval None
  */
static void hc32_uart_get_dma_info(void)
{
#ifdef BSP_USING_UART3
    uart_obj[UART3_INDEX].uart_dma_flag = 0;
#ifdef BSP_UART3_RX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart3_dma_rx = UART3_DMA_RX_CONFIG;
    static struct hc32_uart_rxto uart3_rx_timeout = UART3_RXTO_CONFIG;
    uart3_dma_rx.irq_callback = hc32_uart3_dma_rx_irq_handler;
    uart3_rx_timeout.irq_callback = hc32_uart3_rxto_irq_handler;
    uart_config[UART3_INDEX].rx_timeout = &uart3_rx_timeout;
    uart_config[UART3_INDEX].dma_rx = &uart3_dma_rx;
#endif
#ifdef BSP_UART3_TX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart3_dma_tx = UART3_DMA_TX_CONFIG;
    uart_config[UART3_INDEX].dma_tx = &uart3_dma_tx;
    static struct hc32_uart_irq_config uart3_tc_irq = UART3_TX_CPLT_CONFIG;
    uart3_tc_irq.irq_callback = hc32_uart3_tc_irq_handler;
    uart_config[UART3_INDEX].tc_irq = &uart3_tc_irq;
#endif
#endif
}

/**
  * @brief  This function gets uart irq handle.
  * @param  None
  * @retval None
  */
static void hc32_get_uart_callback(void)
{
#ifdef BSP_USING_UART3
    uart_config[UART3_INDEX].rxerr_irq.irq_callback = hc32_uart3_rxerr_irq_handler;
    uart_config[UART3_INDEX].rx_irq.irq_callback = hc32_uart3_rx_irq_handler;
    uart_config[UART3_INDEX].tx_irq.irq_callback = hc32_uart3_tx_irq_handler;
    uart_config[UART3_INDEX].tx_cplt_irq.irq_callback = hc32_uart3_tx_cplt_irq_handler;
#endif
}

static const struct fm_serial_ops hc32_uart_ops =
{
    .configure = hc32_configure,
    .control = hc32_control,
    .putc = hc32_putc,
    .getc = hc32_getc,
    .transmit = hc32_transmit
};

int fm_hw_usart_init(void)
{
    fm_err_t result = FM_EOK;
    fm_size_t obj_num = sizeof(uart_obj) / sizeof(struct hc32_uart);
    struct serial_configure config = FM_SERIAL_CONFIG_DEFAULT;

    hc32_uart_get_dma_info();
    hc32_get_uart_callback();

    for (int i = 0; i < obj_num; i++)
    {
        /* init UART object */
        uart_obj[i].serial.ops    = &hc32_uart_ops;
        uart_obj[i].serial.config = config;
        uart_obj[i].config = &uart_config[i];
        /* register the handle */
        hc32_install_irq_handler(&uart_config[i].rxerr_irq.irq_config, uart_config[i].rxerr_irq.irq_callback, FM_FALSE);
        hc32_install_irq_handler(&uart_config[i].tx_cplt_irq.irq_config, uart_config[i].tx_cplt_irq.irq_callback, FM_FALSE);
#ifdef FM_SERIAL_USING_DMA
        if (uart_obj[i].uart_dma_flag & FM_DEVICE_FLAG_DMA_RX)
        {
            hc32_install_irq_handler(&uart_config[i].dma_rx->irq_config, uart_config[i].dma_rx->irq_callback, FM_FALSE);
            hc32_install_irq_handler(&uart_config[i].rx_timeout->irq_config, uart_config[i].rx_timeout->irq_callback, FM_FALSE);
        }
        if (uart_obj[i].uart_dma_flag & FM_DEVICE_FLAG_DMA_TX)
        {
            hc32_install_irq_handler(&uart_config[i].tc_irq->irq_config, uart_config[i].tc_irq->irq_callback, FM_FALSE);
        }
#endif
        /* register UART device */
        result = fm_hw_serial_register(&uart_obj[i].serial,
                                        uart_obj[i].config->name,
                                        FM_DEVICE_FLAG_RDWR,
                                        NULL);
        FM_ASSERT(result == FM_EOK);
    }

    return result;
}

#endif

#endif /* FM_USING_SERIAL */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
