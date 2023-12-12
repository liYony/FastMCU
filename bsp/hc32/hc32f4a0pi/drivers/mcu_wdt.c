#include <dal.h>
#include "hc32_ll.h"

static uint8_t is_start = 0;
static stc_wdt_init_t hiwdg;

int mcu_wdt_keep_alive(void)
{
    WDT_FeedDog();
    return 0;
}

/* timeout(s) = PERI * CKS / PCLK3
   16384 * 8192 / 50M = 2.68s
*/
int mcu_wdt_set_timeout(uint32_t timeout)
{
    hiwdg.u32CountPeriod = WDT_CNT_PERIOD16384;
    if (hiwdg.u32RefreshRange > 0xFFFF)
    {
        log_e("wdg set timeout parameter too large, please less than %ds", 0xFFFF);
        return -1;
    }
    if (is_start)
    {
        if (WDT_Init(&hiwdg) != LL_OK)
        {
            log_e("wdg set timeout failed.");
            return -1;
        }
    }
    return 0;
}

uint32_t mcu_wdt_get_timeout(void)
{
    return (uint32_t)(16384 * 8192 / 50000000);
}

int mcu_wdt_start(void)
{
    if (WDT_Init(&hiwdg) != LL_OK)
    {
        log_e("wdt start failed.");
        return -1;
    }
    is_start = 1;
    
    return 0;
}

int mcu_wdt_init(uint32_t timeout)
{
    hiwdg.u32ClockDiv = WDT_CLK_DIV8192;
    hiwdg.u32CountPeriod = WDT_CNT_PERIOD16384;
    hiwdg.u32RefreshRange = WDT_RANGE_0TO100PCT;
    hiwdg.u32LPMCount = WDT_LPM_CNT_STOP;
    hiwdg.u32ExceptionType = WDT_EXP_TYPE_RST;
    is_start = 0;

    return 0;
}

void wdt_init()
{
    dal_wdt_init(0);
    dal_wdt_start();
}

INITLV4_EXPORT(wdt_init);


void wdt_poll()
{
//    dal_wdt_keep_alive();
}

POLLING_EXPORT(wdt_poll);