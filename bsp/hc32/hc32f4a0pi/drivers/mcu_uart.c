#include <dal.h>
#include "hc32_ll.h"

static int cm_uart3_init(uint32_t baudrate)
{
    stc_usart_uart_init_t stcUartInit;
    
    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_10, GPIO_FUNC_33);// RX
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_11, GPIO_FUNC_32);// TX
    /* Enable peripheral clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART3, ENABLE);
    /* Initialize UART. */
    USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32Baudrate = baudrate;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if(LL_OK != USART_UART_Init(CM_USART3, &stcUartInit, NULL))
    {
        return -1;
    }
    /* Enable RX function */
    USART_FuncCmd(CM_USART3, USART_RX, ENABLE);
    USART_FuncCmd(CM_USART3, USART_TX, ENABLE);
    
    return 0;
}

static void UartSendByte(uint8_t dat)
{
    /* If  don't, some data will be garbled and either start byte or terminate byte */
    while(RESET == USART_GetStatus(CM_USART3, USART_FLAG_TX_EMPTY));
    USART_WriteData(CM_USART3, (uint16_t)dat);
    /*UsartTxComplete is must !!! */
    while(RESET == USART_GetStatus(CM_USART3, USART_FLAG_TX_CPLT));
}

static void cm_uart3_send_nbyte(uint8_t *buf, uint16_t len)
{
    //  USART_FuncCmd(USART_CH, USART_RX, ENABLE);
    while(len--)
        UartSendByte(*buf++);
    //    USART_FuncCmd(USART_CH, UsartTx, DISABLE);
}

int mcu_uart_init(dal_uart_number_t uart, uint32_t band)
{
    if (uart == DAL_UART_3)
    {
        return cm_uart3_init(band);
    }
    return -1;
}

int mcu_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len)
{
    if (uart == DAL_UART_3)
    {
        cm_uart3_send_nbyte((uint8_t *)pbuf, len);
        return 0;
    }
    return -1;
}
