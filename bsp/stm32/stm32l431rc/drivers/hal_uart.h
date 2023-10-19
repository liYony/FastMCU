#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "stm32l4xx_hal.h"

void MX_USART1_UART_Init(void);
void uart_send_nbyte(uint8_t *bf, uint16_t len);
#endif // !__HAL_UART_H__
