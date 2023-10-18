#ifndef __QK_HALPORT_H__
#define __QK_HALPORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <qk_type.h>
#include <qkconfig.h>

#ifndef TICK_FRQ_HZ
#define TICK_FRQ_HZ 1000
#endif

#define MS2TICKS(m) (m / (1000 / TICK_FRQ_HZ))
#define TICKS2MS(t) (t * (1000 / TICK_FRQ_HZ))

// 弱函数，用户需要实现这个函数来实现log打印功能
int qk_hal_log_write(const uint8_t *pbuf, uint16_t len);
// 移植时调用，TICK中断服务函数调用qk_hal_inc_systick
void qk_hal_inc_systick(void);
// 弱函数，用户可重新实现此函数。
void qk_hal_user_init(void);

void     qk_hal_init(void);
void     qk_delay_ms(uint16_t xms);
void     qk_delay_us(uint32_t xus);
uint32_t qk_hal_get_systick(void);
uint64_t qk_hal_get_systick_plus(void);

#ifdef __cplusplus
}
#endif

#endif /* __QK_HALPORT_H__ */
