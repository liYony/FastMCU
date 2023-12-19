/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-19     liYony       first version
 */

#include "drv_common.h"
#include "board.h"

#ifdef FM_USING_SERIAL
#include "drv_usart.h"
#endif /* FM_USING_SERIAL */
#ifdef FM_USING_PIN
#include "drv_gpio.h"
#endif /* FM_USING_PIN */

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
uint8_t heap_buffer[4096];

fm_weak void fm_hw_board_init(void)
{
    /* HAL_Init() function is called at the beginning of the program */
    HAL_Init();

    /* System clock initialization */
    SystemClock_Config();

    /* USART driver initialization is open by default */
#ifdef FM_USING_SERIAL
    fm_hw_usart_init();
#endif
    /* GPIO driver initialization is open by default */
#ifdef FM_USING_PIN
    fm_hw_pin_init();
#endif
    /* Set the shell console output device */
#if defined(FM_USING_CONSOLE) && defined(FM_USING_DEVICE)
    fm_console_set_device(FM_CONSOLE_DEVICE_NAME);
#endif

#if defined(FM_USING_HEAP)
    fm_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
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
