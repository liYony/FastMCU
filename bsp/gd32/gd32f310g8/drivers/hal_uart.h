#ifndef __HAL_UART_H
#define __HAL_UART_H

// Files includes
#include "gd32f3x0.h"
#include "stdio.h"
void UartSendByte(uint8_t dat);
void uart_init(uint32_t bound);
void uart_send_nbyte(uint8_t* buf, uint16_t len);
#endif
