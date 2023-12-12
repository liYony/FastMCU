#ifndef __DAL_WDT_H__
#define __DAL_WDT_H__

#include <dal_type.h>

int mcu_wdt_keep_alive(void);
int mcu_wdt_set_timeout(uint32_t timeout);
uint32_t mcu_wdt_get_timeout(void);
int mcu_wdt_start(void);
int mcu_wdt_init(uint32_t timeout);

int dal_wdt_keep_alive(void);
int dal_wdt_set_timeout(uint32_t timeout);
uint32_t dal_wdt_get_timeout(void);
int dal_wdt_start(void);
int dal_wdt_init(uint32_t timeout);

#endif
