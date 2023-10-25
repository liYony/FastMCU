#ifndef __DAL_H__
#define __DAL_H__

#include <qkconfig.h>

#ifdef DAL_SYSTEM_ENABLE
#include <dal_system.h>
#endif /* DAL_SYSTEM_ENABsystem */
#ifdef DAL_SERIAL_ENABLE
#include <dal_uart.h>
#endif /* DAL_SERIAL_ENABLE */
#ifdef DAL_IT_ENABLE
#include <dal_it.h>
#endif /* DAL_IT_ENABLE */
#ifdef DAL_GPIO_ENABLE
#include <dal_gpio.h>
#endif /* DAL_GPIO_ENABLE */
#ifdef DAL_FLASH_ENABLE
#include <dal_flash.h>
#endif /* DAL_FLASH_ENABLE */
#ifdef DAL_TIMER_ENABLE
#include <dal_timer.h>
#endif /* DAL_TIMER_ENABLE */

#endif // !__DAL_H__
