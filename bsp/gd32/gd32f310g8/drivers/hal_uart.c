#include "hal_uart.h"

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

//    nvic_irq_enable(USART0_IRQn,1, 1);
//	usart_interrupt_enable(USART0, USART_INT_RBNE);
//	usart_interrupt_enable(USART0, USART_INT_IDLE);
}
void USART0_IRQHandler(void)
{
}
void UartSendByte(uint8_t dat)
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

