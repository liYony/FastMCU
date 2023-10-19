#include <qk_halport.h>

static volatile uint32_t qk_systick_over   = 0;
static volatile uint32_t qk_systick      = 0;
static uint32_t          qk_us_delay_param = 10;

static void _qk_hal_update_delay_param()
{
    volatile uint32_t delay  = 100000 * qk_us_delay_param;
    volatile uint32_t tick_s = 0, tick_e = 0;
    tick_s = qk_systick;
    while (delay--)
        ;
    tick_e        = qk_systick;
    qk_us_delay_param = (uint32_t)((100.0 / (tick_e - tick_s)) * qk_us_delay_param);
}

__WEAKDEF int qk_hal_log_write(const uint8_t *pbuf, uint16_t len)
{
    return 0;
}

__WEAKDEF void qk_hal_system_init()
{
    ;
}

__WEAKDEF void qk_hal_user_init()
{
    ;
}

void qk_hal_init()
{
    qk_hal_system_init();
    _qk_hal_update_delay_param();
    qk_hal_user_init();
}

void qk_hal_inc_systick()
{
    qk_systick += 1;
    if(qk_systick == 0)
    {
        qk_systick_over += 1;
    }
}

uint32_t qk_hal_get_systick()
{
    return qk_systick;
}

uint64_t qk_hal_get_systick_plus()
{
    uint64_t tick = qk_systick_over;
    tick = qk_systick + (tick << (8 * sizeof(uint32_t)));
    return tick;
}

void qk_delay_ms(uint16_t xms)
{
    uint32_t tickstart = qk_systick;
    uint32_t wait      = MS2TICKS(xms);
    while ((qk_systick - tickstart) < wait)
    {
        ;
    }
}

void qk_delay_us(uint32_t xus)
{
    volatile uint32_t delay = xus * qk_us_delay_param;
    while (delay--)
        ;
}
