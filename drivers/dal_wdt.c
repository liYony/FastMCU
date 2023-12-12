#include <dal_wdt.h>

dal_weak int mcu_wdt_keep_alive(void)
{
    return -1;
}

dal_weak int mcu_wdt_set_timeout(uint32_t timeout)
{
    return -1;
}

dal_weak uint32_t mcu_wdt_get_timeout(void)
{
    return (uint32_t)-1;
}

dal_weak int mcu_wdt_start(void)
{
    return -1;
}

dal_weak int mcu_wdt_init(uint32_t timeout)
{
    return -1;
}

int dal_wdt_keep_alive(void)
{
    return mcu_wdt_keep_alive();
}

int dal_wdt_set_timeout(uint32_t timeout)
{
    return mcu_wdt_set_timeout(timeout);
}

uint32_t dal_wdt_get_timeout(void)
{
    return mcu_wdt_get_timeout();
}

int dal_wdt_start(void)
{
    return mcu_wdt_start();
}

int dal_wdt_init(uint32_t timeout)
{
    return mcu_wdt_init(timeout);
}
