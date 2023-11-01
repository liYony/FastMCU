#ifndef __DAL_SYSTEM_H__
#define __DAL_SYSTEM_H__

#include <dal_type.h>
#include <dal_uart.h>

#ifndef TICK_FRQ_HZ
#define TICK_FRQ_HZ 1000
#endif

#define MS2TICKS(m) (m / (1000 / TICK_FRQ_HZ))
#define TICKS2MS(t) (t * (1000 / TICK_FRQ_HZ))

void dal_system_init(void);
void dal_user_init(void);
void dal_init(void);
void dal_inc_systick(void);
uint32_t dal_get_systick(void);
uint64_t dal_get_systick_plus(void);
void dal_delay_ms(uint16_t xms);
void dal_delay_us(uint32_t xus);

void dal_set_log_output(dal_uart_number_t uart);
void dal_log_output(uint8_t *pbuf, uint16_t len);
int dal_log_input(uint8_t *pbuf, uint16_t len);

#endif // !__DAL_SYSTEM_H__
