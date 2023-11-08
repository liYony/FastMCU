#include <dal_system.h>

static volatile uint32_t systick_over   = 0;
static volatile uint32_t systick      = 0;
static uint32_t          us_delay_param = 10;
static dal_uart_number_t log_uart = DAL_UART_NUMBER;

dal_weak void dal_system_init()
{
    ;
}

dal_weak void dal_user_init()
{
    ;
}

static void _dal_update_delay_param()
{
    volatile uint32_t delay  = 100000 * us_delay_param;
    volatile uint32_t tick_s = 0, tick_e = 0;
    tick_s = systick;
    while (delay--)
        ;
    tick_e        = systick;
    us_delay_param = (uint32_t)((100.0 / (tick_e - tick_s)) * us_delay_param);
}

void dal_init()
{
    dal_system_init();
    _dal_update_delay_param();
    dal_user_init();
}

void dal_inc_systick()
{
    systick += 1;
    if(systick == 0)
    {
        systick_over += 1;
    }
}

uint32_t dal_get_systick()
{
    return systick;
}

uint64_t dal_get_systick_plus()
{
    uint64_t tick = systick_over;
    tick = systick + (tick << (8 * sizeof(uint32_t)));
    return tick;
}

void dal_delay_ms(uint16_t xms)
{
    uint32_t tickstart = systick;
    uint32_t wait      = MS2TICKS(xms);
    while ((systick - tickstart) < wait)
    {
        ;
    }
}

void dal_delay_us(uint32_t xus)
{
    volatile uint32_t delay = xus * us_delay_param;
    while (delay--)
        ;
}

//================================================================================================//
//================================================================================================//

void dal_set_log_uart(dal_uart_number_t uart)
{
    log_uart = uart;
}

void dal_log_output(uint8_t *pbuf, uint16_t len)
{
    if (log_uart == DAL_UART_NUMBER)
    {
        return;
    }
    dal_uart_send(log_uart, pbuf, len);
}

int dal_log_input(uint8_t *pbuf, uint16_t len)
{
    if (log_uart == DAL_UART_NUMBER)
    {
        return -1;
    }
    return dal_uart_receive(log_uart, pbuf, len);
}
