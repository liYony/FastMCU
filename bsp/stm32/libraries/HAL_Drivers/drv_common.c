#include "drv_common.h"
#include "board.h"

#define DBG_TAG    "drv_common"
#define DBG_LVL    DBG_INFO
#include <fmdbg.h>

/* SysTick configuration */
void rt_hw_systick_init(void)
{
    HAL_SYSTICK_Config(SystemCoreClock / FM_TICK_PER_SECOND);

    NVIC_SetPriority(SysTick_IRQn, 0xFF);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        HAL_IncTick();

    fm_inc_systick();
}

/* re-implement tick interface for STM32 HAL */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    rt_hw_systick_init();

    /* Return function status */
    return HAL_OK;
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

UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        return;
    }
}

uint8_t heap_buffer[4096];

fm_weak void fm_hw_board_init(void)
{
    /* HAL_Init() function is called at the beginning of the program */
    HAL_Init();

    /* System clock initialization */
    SystemClock_Config();

    MX_USART1_UART_Init();

#if defined(FM_USING_HEAP)
    fm_system_heap_init((void *)heap_buffer, (void *)(heap_buffer + sizeof(heap_buffer)));
#endif

    /* Board underlying hardware initialization */
#ifdef FM_USING_COMPONENTS_INIT
    fm_components_board_init();
#endif
}

void fm_hw_console_output(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 100);
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
