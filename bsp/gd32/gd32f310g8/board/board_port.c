#include <qk_halport.h>
#include <qk_section.h>
#include "hal_uart.h"

#define LOG_LVL LVL_DEBUG
#include "qk_log.h"

int qk_hal_log_write(const uint8_t *pbuf, uint16_t len)
{
    uart_send_nbyte((uint8_t *)pbuf, len);
    return 0;
}

void qk_hal_system_init(void)
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

void board_init(void)
{
    qk_hal_init();
    
    uart_init(230400);
    
    log_d("board init.\r\n");
}

INITLV0_EXPORT(board_init);

void SysTick_Handler(void)
{
    qk_hal_inc_systick();
}
