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
 * 2023-12-19     liYony       adapt FastMCU complete, but now dma is not supported
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

#define DMA_CH_REG(reg_base, ch)                                               \
    (*(uint32_t *)((uint32_t)(&(reg_base)) + ((ch) * 0x40UL)))

#define DMA_TRANS_CNT(unit, ch)                                                \
    (READ_REG32(DMA_CH_REG((unit)->MONDTCTL0, (ch))) >> DMA_DTCTL_CNT_POS)

#define USART_TCI_ENABLE(unit)                                                 \
    SET_REG32_BIT(unit->CR1, USART_INT_TX_CPLT)


#define UART_BAUDRATE_ERR_MAX           (0.025F)

#if defined (HC32F460)
    #define FCG_USART_CLK               FCG_Fcg1PeriphClockCmd

#elif defined (HC32F4A0)
    #define FCG_USART_CLK               FCG_Fcg3PeriphClockCmd
#endif

#define FCG_TMR0_CLK                    FCG_Fcg2PeriphClockCmd
#define FCG_DMA_CLK                     FCG_Fcg0PeriphClockCmd

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
extern fm_err_t fm_hw_board_uart_init(CM_USART_TypeDef *USARTx);

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
#ifdef FM_SERIAL_USING_DMA
    static void hc32_dma_config(struct fm_serial_device *serial, fm_ubase_t flag);
#endif

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

#ifdef FM_SERIAL_USING_DMA
    uart->dmarx_remaining_cnt = serial->config.rx_bufsz;
#endif

    /* Enable completion interrupt */
    NVIC_EnableIRQ(uart->config->tx_cplt_irq.irq_config.irq_num);
    /* Enable error interrupt */
    NVIC_EnableIRQ(uart->config->rx_err_irq.irq_config.irq_num);
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
#ifdef FM_SERIAL_USING_DMA
        else if (FM_DEVICE_FLAG_DMA_RX == ctrl_arg)
        {
            NVIC_DisableIRQ(uart->config->dma_rx->irq_config.irq_num);
        }
        else if (FM_DEVICE_FLAG_DMA_TX == ctrl_arg)
        {
            NVIC_DisableIRQ(uart->config->dma_tx->irq_config.irq_num);
        }
#endif
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
#ifdef FM_SERIAL_USING_DMA
            hc32_dma_config(serial, ctrl_arg);
#endif
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
    FM_ASSERT(serial != FM_NULL);
    FM_ASSERT(buf != FM_NULL);

#ifdef BSP_UART3_TX_USING_DMA
    struct hc32_uart *uart;
    struct dma_config *uart_dma;
    uart = fm_container_of(serial, struct hc32_uart, serial);
    if (uart->uart_dma_flag & FM_DEVICE_FLAG_DMA_TX)
    {
        uart_dma = uart->config->dma_tx;
        if (RESET == USART_GetStatus(uart->config->Instance, USART_FLAG_TX_CPLT))
        {
            FM_ASSERT(0);
        }
        DMA_SetSrcAddr(uart_dma->Instance, uart_dma->channel, (uint32_t)buf);
        DMA_SetTransCount(uart_dma->Instance, uart_dma->channel, size);
        DMA_ChCmd(uart_dma->Instance, uart_dma->channel, ENABLE);
        USART_FuncCmd(uart->config->Instance, USART_TX, ENABLE);
        USART_FuncCmd(uart->config->Instance, USART_INT_TX_CPLT, ENABLE);
        return size;
    }
#endif

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

static void hc32_uart_rx_err_irq_handler(struct hc32_uart *uart)
{
    FM_ASSERT(FM_NULL != uart);
    FM_ASSERT(FM_NULL != uart->config->Instance);

    if (SET == USART_GetStatus(uart->config->Instance, (USART_FLAG_OVERRUN | USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR)))
    {
        USART_ReadData(uart->config->Instance);
    }
    USART_ClearStatus(uart->config->Instance, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

#ifdef FM_SERIAL_USING_DMA
static void hc32_uart_rx_timeout(struct fm_serial_device *serial)
{
    struct hc32_uart *uart;
    uint32_t cmp_val;
    CM_TMR0_TypeDef *TMR0_Instance;
    uint8_t ch;
    uint32_t timeout_bits;
    stc_tmr0_init_t stcTmr0Init;

    FM_ASSERT(RT_NULL != serial);
    uart = fm_container_of(serial, struct hc32_uart, serial);
    FM_ASSERT(FM_NULL != uart->config->Instance);

    TMR0_Instance = uart->config->rx_timeout->TMR0_Instance;
    ch            = uart->config->rx_timeout->channel;
    timeout_bits  = uart->config->rx_timeout->timeout_bits;
#if defined (HC32F460)
    if ((CM_USART1 == uart->config->Instance) || (CM_USART3 == uart->config->Instance))
    {
        RT_ASSERT(TMR0_CH_A == ch);
    }
    else if ((CM_USART2 == uart->config->Instance) || (CM_USART4 == uart->config->Instance))
    {
        RT_ASSERT(TMR0_CH_B == ch);
    }
#elif defined (HC32F4A0)
    if ((CM_USART1 == uart->config->Instance) || (CM_USART6 == uart->config->Instance))
    {
        FM_ASSERT(TMR0_CH_A == ch);
    }
    else if ((CM_USART2 == uart->config->Instance) || (CM_USART7 == uart->config->Instance))
    {
        FM_ASSERT(TMR0_CH_B == ch);
    }
#endif

    FCG_TMR0_CLK(uart->config->rx_timeout->clock, ENABLE);

    /* TIMER0 basetimer function initialize */
    TMR0_DeInit(TMR0_Instance);
    TMR0_SetCountValue(TMR0_Instance, ch, 0U);
    TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV1;
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_XTAL32;
    if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDiv)
    {
        cmp_val = (timeout_bits - 4UL);
    }
    else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDiv)
    {
        cmp_val = (timeout_bits / 2UL - 2UL);
    }
    else
    {
        cmp_val = (timeout_bits / (1UL << (stcTmr0Init.u32ClockDiv >> TMR0_BCONR_CKDIVA_POS)) - 1UL);
    }
    DDL_ASSERT(cmp_val <= 0xFFFFUL);
    stcTmr0Init.u16CompareValue = (uint16_t)(cmp_val);
    TMR0_Init(TMR0_Instance, ch, &stcTmr0Init);
    TMR0_HWStartCondCmd(TMR0_Instance, ch, ENABLE);
    TMR0_HWClearCondCmd(TMR0_Instance, ch, ENABLE);
    /* Clear compare flag */
    TMR0_ClearStatus(TMR0_Instance, (uint32_t)(0x1UL << ch));

    NVIC_EnableIRQ(uart->config->rx_timeout->irq_config.irq_num);
    USART_ClearStatus(uart->config->Instance, USART_FLAG_RX_TIMEOUT);
    USART_FuncCmd(uart->config->Instance, (USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT), ENABLE);
}

static void hc32_dma_config(struct fm_serial_device *serial, fm_ubase_t flag)
{
    struct hc32_uart *uart;
    stc_dma_init_t dma_init;
    struct dma_config *uart_dma;

    FM_ASSERT(FM_NULL != serial);

    uart = fm_container_of(serial, struct hc32_uart, serial);
    FM_ASSERT(RT_NULL != uart->config->Instance);
    if (FM_DEVICE_FLAG_DMA_RX == flag)
    {
        stc_dma_llp_init_t llp_init;
        struct fm_serial_rx_fifo *rx_fifo = (struct fm_serial_rx_fifo *)serial->serial_rx;

        FM_ASSERT(FM_NULL != uart->config->rx_timeout->TMR0_Instance);
        FM_ASSERT(FM_NULL != uart->config->dma_rx->Instance);

        uart_dma = uart->config->dma_rx;
        /* Initialization uart rx timeout for DMA */
        hc32_uart_rx_timeout(serial);
        /* Enable DMA clock */
        FCG_DMA_CLK(uart_dma->clock, ENABLE);
        DMA_ChCmd(uart_dma->Instance, uart_dma->channel, DISABLE);

        /* Initialize DMA */
        DMA_StructInit(&dma_init);
        dma_init.u32IntEn       = DMA_INT_ENABLE;
        dma_init.u32SrcAddr     = ((uint32_t)(&uart->config->Instance->DR) + 2UL);
        dma_init.u32DestAddr    = (uint32_t)rx_fifo->buffer;
        dma_init.u32DataWidth   = DMA_DATAWIDTH_8BIT;
        dma_init.u32BlockSize   = 1UL;
        dma_init.u32TransCount  = serial->config.rx_bufsz;
        dma_init.u32SrcAddrInc  = DMA_SRC_ADDR_FIX;
        dma_init.u32DestAddrInc = DMA_DEST_ADDR_INC;
        DMA_Init(uart_dma->Instance, uart_dma->channel, &dma_init);

        /* Initialize LLP */
        llp_init.u32State   = DMA_LLP_ENABLE;
        llp_init.u32Mode    = DMA_LLP_WAIT;
        llp_init.u32Addr    = (uint32_t)&uart->config->llp_desc;
        DMA_LlpInit(uart_dma->Instance, uart_dma->channel, &llp_init);

        /* Configure LLP descriptor */
        uart->config->llp_desc.SARx  = dma_init.u32SrcAddr;
        uart->config->llp_desc.DARx  = dma_init.u32DestAddr;
        uart->config->llp_desc.DTCTLx = (dma_init.u32TransCount << DMA_DTCTL_CNT_POS) | (dma_init.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);
        uart->config->llp_desc.LLPx  = (uint32_t)&uart->config->llp_desc;
        uart->config->llp_desc.CHCTLx = (dma_init.u32SrcAddrInc | dma_init.u32DestAddrInc | dma_init.u32DataWidth | \
                           llp_init.u32State      | llp_init.u32Mode        | dma_init.u32IntEn);

        /* Enable DMA interrupt */
        NVIC_EnableIRQ(uart->config->dma_rx->irq_config.irq_num);
        /* Enable DMA module */
        DMA_Cmd(uart_dma->Instance, ENABLE);
        AOS_SetTriggerEventSrc(uart_dma->trigger_select, uart_dma->trigger_event);
        DMA_ChCmd(uart_dma->Instance, uart_dma->channel, ENABLE);
    }
    else if (FM_DEVICE_FLAG_DMA_TX == flag)
    {
        FM_ASSERT(FM_NULL != uart->config->dma_tx->Instance);

        uart_dma = uart->config->dma_tx;
        /* Enable DMA clock */
        FCG_DMA_CLK(uart_dma->clock, ENABLE);
        DMA_ChCmd(uart_dma->Instance, uart_dma->channel, DISABLE);

        /* Initialize DMA */
        DMA_StructInit(&dma_init);
        dma_init.u32IntEn       = DMA_INT_DISABLE;
        dma_init.u32SrcAddr     = 0UL;
        dma_init.u32DestAddr    = (uint32_t)(&uart->config->Instance->DR);
        dma_init.u32DataWidth   = DMA_DATAWIDTH_8BIT;
        dma_init.u32BlockSize   = 1UL;
        dma_init.u32TransCount  = 0UL;
        dma_init.u32SrcAddrInc  = DMA_SRC_ADDR_INC;
        dma_init.u32DestAddrInc = DMA_DEST_ADDR_FIX;
        DMA_Init(uart_dma->Instance, uart_dma->channel, &dma_init);

        /* Enable DMA module */
        DMA_Cmd(uart_dma->Instance, ENABLE);
        AOS_SetTriggerEventSrc(uart_dma->trigger_select, uart_dma->trigger_event);
        USART_FuncCmd(uart->config->Instance, USART_TX | USART_INT_TX_CPLT, DISABLE);
        NVIC_EnableIRQ(uart->config->tx_cplt_irq.irq_config.irq_num);
    }
}

#if defined(BSP_UART1_RX_USING_DMA) || defined(BSP_UART2_RX_USING_DMA) || defined(BSP_UART3_RX_USING_DMA) || \
    defined(BSP_UART4_RX_USING_DMA) || defined(BSP_UART6_RX_USING_DMA) || defined(BSP_UART7_RX_USING_DMA)
static void hc32_uart_dma_rx_irq_handler(struct hc32_uart *uart)
{

}

static void hc32_uart_rxto_irq_handler(struct hc32_uart *uart)
{
    TMR0_Stop(uart->config->rx_timeout->TMR0_Instance, uart->config->rx_timeout->channel);
    USART_ClearStatus(uart->config->Instance, USART_FLAG_RX_TIMEOUT);
}
#endif
#endif /* FM_SERIAL_USING_DMA */

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

static void hc32_uart3_rx_err_irq_handler(void)
{
    hc32_uart_rx_err_irq_handler(&uart_obj[UART3_INDEX]);
}

#if defined(BSP_UART3_RX_USING_DMA)
static void hc32_uart3_rxto_irq_handler(void)
{
    hc32_uart_rxto_irq_handler(&uart_obj[UART3_INDEX]);
}

static void hc32_uart3_dma_rx_irq_handler(void)
{
    hc32_uart_dma_rx_irq_handler(&uart_obj[UART3_INDEX]);
}
#endif /* BSP_UART3_RX_USING_DMA */
#endif /* BSP_USING_UART3 */

static void hc32_uart_get_config(void)
{
    struct serial_configure config = FM_SERIAL_CONFIG_DEFAULT;
#ifdef BSP_USING_UART3
    uart_obj[UART3_INDEX].serial.config = config;
    uart_obj[UART3_INDEX].uart_dma_flag = 0;

    uart_obj[UART3_INDEX].serial.config.rx_bufsz = BSP_UART3_RX_BUFSIZE;
    uart_obj[UART3_INDEX].serial.config.tx_bufsz = BSP_UART3_TX_BUFSIZE;

#ifdef BSP_UART3_RX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_RX;
    static struct dma_config uart3_dma_rx = UART3_DMA_RX_CONFIG;
    static struct hc32_uart_rxto uart3_rx_timeout = UART3_RXTO_CONFIG;
    uart3_dma_rx.irq_callback = hc32_uart3_dma_rx_irq_handler;
    uart3_rx_timeout.irq_callback = hc32_uart3_rxto_irq_handler;
    uart_config[UART3_INDEX].rx_timeout = &uart3_rx_timeout;
    uart_config[UART3_INDEX].dma_rx = &uart3_dma_rx;
#endif /* BSP_UART3_RX_USING_DMA */
#ifdef BSP_UART3_TX_USING_DMA
    uart_obj[UART3_INDEX].uart_dma_flag |= FM_DEVICE_FLAG_DMA_TX;
    static struct dma_config uart3_dma_tx = UART3_DMA_TX_CONFIG;
    uart_config[UART3_INDEX].dma_tx = &uart3_dma_tx;
#endif /* BSP_UART3_TX_USING_DMA */
    uart_config[UART3_INDEX].rx_err_irq.irq_callback = hc32_uart3_rx_err_irq_handler;
    uart_config[UART3_INDEX].rx_irq.irq_callback = hc32_uart3_rx_irq_handler;
    uart_config[UART3_INDEX].tx_irq.irq_callback = hc32_uart3_tx_irq_handler;
    uart_config[UART3_INDEX].tx_cplt_irq.irq_callback = hc32_uart3_tx_cplt_irq_handler;
#endif /* BSP_USING_UART3 */
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

    hc32_uart_get_config();

    for (int i = 0; i < obj_num; i++)
    {
        /* init UART object */
        uart_obj[i].config = &uart_config[i];
        uart_obj[i].serial.ops    = &hc32_uart_ops;

        /* register the handle */
        hc32_install_irq_handler(&uart_config[i].rx_err_irq.irq_config, uart_config[i].rx_err_irq.irq_callback, FM_FALSE);
        hc32_install_irq_handler(&uart_config[i].tx_cplt_irq.irq_config, uart_config[i].tx_cplt_irq.irq_callback, FM_FALSE);
#ifdef FM_SERIAL_USING_DMA
        if (uart_obj[i].uart_dma_flag & FM_DEVICE_FLAG_DMA_RX)
        {
            hc32_install_irq_handler(&uart_config[i].dma_rx->irq_config, uart_config[i].dma_rx->irq_callback, FM_FALSE);
            hc32_install_irq_handler(&uart_config[i].rx_timeout->irq_config, uart_config[i].rx_timeout->irq_callback, FM_FALSE);
        }
#endif /* FM_SERIAL_USING_DMA */
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
