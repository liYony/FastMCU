#include "drv_common.h"
#include "board.h"

#ifdef FM_USING_SERIAL
#include "drv_usart.h"
#endif /*  */
#define DBG_TAG    "drv_common"
#define DBG_LVL    DBG_INFO
#include <fmdbg.h>

/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE               (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                                         LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP               (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)

/**
 * Turn on Flash acceleration, otherwise the program runs much slower than 240Mhz.
 *
 */
static void FlashCache_Enable(void)
{
    EFM_REG_Unlock();
    EFM_PrefetchCmd(ENABLE);
    while(SET != EFM_GetStatus(EFM_FLAG_RDY));

    EFM_DCacheCmd(ENABLE);
    while(SET != EFM_GetStatus(EFM_FLAG_RDY));

    EFM_ICacheCmd(ENABLE);
    while(SET != EFM_GetStatus(EFM_FLAG_RDY));

    EFM_REG_Lock();
}

/** Peripheral Clock Configuration
*/
static void PeripheralClock_Config(void)
{
#if defined(HC32F4A0)
#if defined(BSP_USING_CAN1)
    CLK_SetCANClockSrc(CLK_CAN1, CLK_CANCLK_SYSCLK_DIV6);
#endif
#if   defined(BSP_USING_CAN2)
    CLK_SetCANClockSrc(CLK_CAN2, CLK_CANCLK_SYSCLK_DIV6);
#endif

#if defined(FM_USING_ADC)
    CLK_SetPeriClockSrc(CLK_PERIPHCLK_PCLK);
#endif
#endif
}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
    stc_clock_freq_t stcClkFreq;
    fm_uint32_t cnts;

    CLK_GetClockFreq(&stcClkFreq);

    cnts = (fm_uint32_t)stcClkFreq.u32HclkFreq / FM_TICK_PER_SECOND;

    SysTick_Config(cnts);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    fm_inc_systick();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char *s, int num)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    LOG_E("Error_Handler at file:%s num:%d", s, num);
    while (1)
    {
    }
    /* USER CODE END Error_Handler */
}

/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/

uint8_t heap_buffer[4096];

fm_weak void fm_hw_board_init(void)
{
    /* Peripheral registers write unprotected */
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);

    /* System clock initialization */
    SystemClock_Config();
    FlashCache_Enable();
    PeripheralClock_Config();
    /* Configure the SysTick */
    SysTick_Configuration();

    /* USART driver initialization is open by default */
#ifdef FM_USING_SERIAL
    fm_hw_usart_init();
#endif

    /* Set the shell console output device */
#if defined(FM_USING_CONSOLE) && defined(FM_USING_DEVICE)
    fm_console_set_device(FM_CONSOLE_DEVICE_NAME);
#endif

#if defined(FM_USING_HEAP)
    fm_system_heap_init((void *)heap_buffer, (void *)(heap_buffer + sizeof(heap_buffer)));
#endif

    /* Board underlying hardware initialization */
#ifdef FM_USING_COMPONENTS_INIT
    fm_components_board_init();
#endif
}

fm_base_t fm_hw_interrupt_disable(void)
{
    fm_base_t level = __get_PRIMASK();
    __disable_irq();
    return level;
}

void fm_hw_interrupt_enable(fm_base_t level)
{
    __set_PRIMASK(level);
}

/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
