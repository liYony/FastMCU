#include <fmcu.h>

static volatile fm_uint32_t systick_over   = 0;
static volatile fm_uint32_t systick        = 0;
static fm_uint32_t          us_delay_param = 10;

void fm_update_delay_param(void)
{
    volatile fm_uint32_t delay  = 100000 * us_delay_param;
    volatile fm_uint32_t tick_s = 0, tick_e = 0;
    tick_s = systick;
    while (delay--)
        ;
    tick_e        = systick;
    us_delay_param = (fm_uint32_t)((100.0 / (tick_e - tick_s)) * us_delay_param);
}

void fm_inc_systick(void)
{
    systick += 1;
    if(systick == 0)
    {
        systick_over += 1;
    }
}

fm_uint32_t fm_get_systick(void)
{
    return systick;
}

fm_uint64_t fm_get_systick_plus(void)
{
    fm_uint64_t tick = systick_over;
    tick = systick + (tick << (8 * sizeof(fm_uint32_t)));
    return tick;
}

fm_weak void fm_delay_ms(fm_uint16_t xms)
{
    fm_uint32_t tickstart = systick;
    fm_uint32_t wait      = FM_MS2TICKS(xms);
    while ((systick - tickstart) < wait)
    {
        ;
    }
}

fm_weak void fm_delay_us(fm_uint32_t xus)
{
    volatile fm_uint32_t delay = xus * us_delay_param;
    while (delay--)
        ;
}
