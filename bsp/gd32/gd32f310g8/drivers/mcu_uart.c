#include <dal.h>
#include "gd32f3x0.h"

void uart_init(uint32_t bound)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
	
    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);                                              //Ê¹ÄÜ´®¿Ú1
	
	/* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, bound);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);

    nvic_irq_enable(USART0_IRQn,1, 1);
	usart_interrupt_enable(USART0, USART_INT_RBNE);
    
//	usart_interrupt_enable(USART0, USART_INT_IDLE);
}

static void UartSendByte(uint8_t dat)
{
    usart_data_transmit(USART0, (uint8_t) dat);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
}
void uart_send_nbyte(uint8_t* buf, uint16_t len)
{
	#if(_DEBUG_OUT_ == 0)
    while(len--)
        UartSendByte(*buf++);
	#else
	extern void delay_ms(uint32_t mtime);
	delay_ms(5);
	while(len--)
        UartSendByte(*buf++);
	delay_ms(5);
	#endif
}

int dal_uart0_idle_cb(uint8_t *pbuf, uint16_t len, void *user_data)
{
    qk_kprintf("idle\r\n");
//    uart_send_nbyte(pbuf, len);
//    qk_kprintf("\r\n");
    
    return 0;
}
DAL_UART_CREATE_ATTR(uart0, 256, 40, dal_uart0_idle_cb, NULL);
uint8_t uart1_r_data;
void USART0_IRQHandler(void)
{
    if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) == SET)
    {
        uart1_r_data = usart_data_receive(USART0);
		dal_it_param_t p;
        p._uart.pbuf = &uart1_r_data;
        p._uart.len = 1;
        dal_it_invoke(DAL_HAL_IT_UART_RX, DAL_UART_0, &p);
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
        
	}
}



int mcu_uart_init(dal_uart_number_t uart, uint32_t band)
{
    if (uart == DAL_UART_0)
    {
        uart_init(115200);
        dal_uart_idle_attach_irq(DAL_UART_0, &uart0);
        return 0;
    }

    return -1;
}

int mcu_uart_deinit(dal_uart_number_t uart)
{
    return -1;
}

int mcu_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len)
{
    if (uart == DAL_UART_0)
    {
        uart_send_nbyte((uint8_t *)pbuf, len);
        return 0;
    }

    return -1;
}

int mcu_uart_receive(dal_uart_number_t uart, uint8_t *pbuf, uint16_t len)
{
    return -1;
}