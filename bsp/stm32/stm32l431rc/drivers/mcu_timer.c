#include <dal.h>
#include "stm32l4xx_hal.h"

TIM_HandleTypeDef timer2 = {.Instance = TIM2};
TIM_HandleTypeDef timer7 = {.Instance = TIM7};

static uint32_t mcu_timer_prescaler(uint32_t tim_base, uint32_t freq)
{
    uint32_t flatency = 0;
    uint32_t prescaler_value = 0;
    uint32_t pclk1_doubler = 1, pclk2_doubler = 1;
    
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &flatency);

    if (RCC_ClkInitStruct.APB1CLKDivider != RCC_HCLK_DIV1)
    {
         pclk1_doubler = 2;
    }
    if (RCC_ClkInitStruct.APB2CLKDivider != RCC_HCLK_DIV1)
    {
         pclk2_doubler = 2;
    }
    
    if ((uint32_t)tim_base >= APB2PERIPH_BASE)
    {
        prescaler_value = (uint32_t)(HAL_RCC_GetPCLK2Freq() * pclk2_doubler / freq) - 1;
    }
    else
    {
        prescaler_value = (uint32_t)(HAL_RCC_GetPCLK1Freq() * pclk1_doubler / freq) - 1;
    }
    
    log_d("prescaler_value = %d\r\n", prescaler_value);
    
    return prescaler_value;
}

static void timer2_init(dal_timer_cntmode_t cntm, uint32_t freq)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    timer2.Instance = TIM2;
    
    if (cntm == DAL_TIMER_CNTMODE_UP)
    {
        timer2.Init.CounterMode   = TIM_COUNTERMODE_UP;
    }
    else
    {
        timer2.Init.CounterMode   = TIM_COUNTERMODE_DOWN;
    }
    timer2.Init.Prescaler = mcu_timer_prescaler((uint32_t)TIM2, freq);
    timer2.Init.Period = 65535;
    timer2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer2.Init.RepetitionCounter = 0;
    timer2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&timer2) != HAL_OK)
    {
        return;
    }
    
    HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0); /* set the TIMx priority */
    HAL_NVIC_EnableIRQ(TIM2_IRQn); /* enable the TIMx global Interrupt */
    __HAL_TIM_CLEAR_FLAG(&timer2, TIM_FLAG_UPDATE); /* clear update flag */
    __HAL_TIM_URS_ENABLE(&timer2); /* enable update request source */ 
}

void mcu_timer_init(dal_timer_number_t timer, dal_timer_cntmode_t cntm, uint32_t freq)
{
    if (timer == DAL_TIMER_1)
    {
        timer2_init(cntm, freq);
    }
}

void mcu_timer_start(dal_timer_number_t timer, dal_timer_mode_t mode, uint32_t period)
{
    /* set tim cnt */
    __HAL_TIM_SET_COUNTER(&timer2, 0);
    /* set tim arr */
    __HAL_TIM_SET_AUTORELOAD(&timer2, period - 1);

    if (mode == DAL_TIMER_MODE_ONESHOT)
    {
        /* set timer to single mode */
        timer2.Instance->CR1 |= TIM_OPMODE_SINGLE;
    }
    else
    {
        timer2.Instance->CR1 &= (~TIM_OPMODE_SINGLE);
    }

    /* start timer */
    if (HAL_TIM_Base_Start_IT(&timer2) != HAL_OK)
    {
        return;
    }
}

void mcu_timer_stop(dal_timer_number_t timer)
{
    /* stop timer */
    HAL_TIM_Base_Stop_IT(&timer2);

    /* set tim cnt */
    __HAL_TIM_SET_COUNTER(&timer2, 0);
}


#include <qk_section.h>

DAL_TIMER_CREATE_ATTR(timer2_irq, DAL_TIMER_2, NULL);

void timer2_func(dal_it_number_t it, uint8_t index, dal_it_param_t *param, void *user_data)
{
    log_d("irq cb %d %d\r\n", it, index);
}

void timer_test(void)
{
    dal_timer_init(DAL_TIMER_1, DAL_TIMER_CNTMODE_UP, 10000);
    dal_timer_start(DAL_TIMER_1, DAL_TIMER_MODE_PERIOD, 10000);
    
    dal_timer_attach_irq(&timer2_irq, timer2_func);
}

INITLV4_EXPORT(timer_test);

void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&timer2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == timer2.Instance) 
	{
		dal_it_invoke(DAL_HAL_IT_TIMER_FLOW, DAL_TIMER_2, NULL);
	}
}
/* USER CODE END 1 */

