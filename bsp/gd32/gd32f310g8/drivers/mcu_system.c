#include <dal.h>
#include "gd32f3x0.h"

void dal_system_init()
{
    if (SysTick_Config(SystemCoreClock / TICK_FRQ_HZ))
    {
        /* capture error */
        while (1)
        {
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void SysTick_Handler(void)
{
    dal_inc_systick();
}

void dal_user_init()
{
    dal_uart_init(DAL_UART_0, 230400);
    dal_set_log_uart(DAL_UART_0);
}
