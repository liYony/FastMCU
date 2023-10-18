////////////////////////////////////////////////////////////////////////////////
/// @file    delay.c
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion
#define _DELAY_C_

// Files includes
#include "hal_delay.h"
////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DELAY
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup DELAY_Exported_Functions
/// @{

#define MAX_DELAY       0xFFFFFFFFU
extern uint32_t SystemCoreClock;
__IO uint32_t uwTick;

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initialize systick for delay function
/// @note   This function should affected by chip version /8 or /1.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void delay_init(void)
{
  /* setup systick timer for 1000Hz interrupts */
  if (SysTick_Config(SystemCoreClock / 1000U))
  {
    /* capture error */
    while (1)
    {
    }
  }
  /* configure the systick handler priority */
  NVIC_SetPriority(SysTick_IRQn, 0x00U);

#if USE_DELAYUS_TIMER
  delay_us_init();
#endif
}

/**
 * @brief This function is called to increment a global variable "uwTick"
 *        used as application time base.
 * @note In the default implementation, this variable is incremented each 1ms
 *       in SysTick ISR.
 * @note This function is declared as __weak to be overwritten in case of other
 *      implementations in user file.
 * @retval None
 */
void sys_inctick(void)
{
	uwTick++;
}

/**
 * @brief Provides a tick value in millisecond.
 * @note This function is declared as __weak to be overwritten in case of other
 *       implementations in user file.
 * @retval tick value
 */
uint32_t sys_gettick(void)
{
  return uwTick;
}

/**
 * @brief This function provides minimum delay (in milliseconds) based
 *        on variable incremented.
 * @note In the default implementation , SysTick timer is the source of time base.
 *       It is used to generate interrupts at regular time intervals where uwTick
 *       is incremented.
 * @note This function is declared as __weak to be overwritten in case of other
 *       implementations in user file.
 * @param Delay specifies the delay time length, in milliseconds.
 * @retval None
 */
void delay_ms(uint32_t mtime)
{
  uint32_t tickstart = sys_gettick();
  uint32_t wait = mtime;

  /* Add a period to guaranty minimum wait */
  if (wait < MAX_DELAY)
  {
    wait++;
  }

  while ((sys_gettick() - tickstart) < wait)
  {
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SysTick_Handler is call from interrupt map
/// @note   Call delay count function.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
//void SysTick_Handler(void)
//{
//  sys_inctick();
//}

#if (USE_DELAYUS_TIMER)
#define DELAY_TIMER TIMER13
#define RCU_DELAY_TIMER RCU_TIMER13
#define DELAY_TIMER_IRQHandler TIMER13_IRQHandler
#define DELAY_TIMER_IRQn TIMER13_IRQn

//定时器不分频
#define DELAY_TIMER_PRESCALER 0 //分频值0,频率和系统时钟一样
// 1us的定时计数是
#define DELAY_TIMER_PERIOD (SystemCoreClock / 1000 / 1000) - 1 // 71	//SystemCoreClock / 1000 /1000

#define TMR_ENABLE() TIMER_CTL0(DELAY_TIMER) |= (uint32_t)TIMER_CTL0_CEN        //开启定时器
#define TMR_DISABEL() TIMER_CTL0(DELAY_TIMER) &= ~(uint32_t)TIMER_CTL0_CEN      //关闭定时器
#define TMR_CLR_FLAG() TIMER_INTF(DELAY_TIMER) = (~(uint32_t)TIMER_INT_FLAG_UP) //清除中断标志位
#define TMR_SET_CNT(X) TIMER_CNT(DELAY_TIMER) = (uint32_t)(X)                   //配置计数器

uint16_t us_count;

/*----------------------------------------------------------------------------------
Function   :Delay_Timer_Init
Description:初始化延时函数定时器
Input      :无
Retrun     :无
----------------------------------------------------------------------------------*/
void delay_us_init(void)
{
  timer_parameter_struct tmr;

  rcu_periph_clock_enable(RCU_DELAY_TIMER);
  timer_deinit(DELAY_TIMER);
  timer_struct_para_init(&tmr);
  /* TIMERx configuration */
  tmr.prescaler = DELAY_TIMER_PRESCALER;
  tmr.alignedmode = TIMER_COUNTER_EDGE;
  tmr.counterdirection = TIMER_COUNTER_UP;
  tmr.period = DELAY_TIMER_PERIOD;
  tmr.clockdivision = TIMER_CKDIV_DIV1;
  tmr.repetitioncounter = 0;
  timer_init(DELAY_TIMER, &tmr);

  timer_interrupt_flag_clear(DELAY_TIMER, TIMER_INT_UP);
  timer_interrupt_enable(DELAY_TIMER, TIMER_INT_UP); //'????????
  nvic_irq_enable(DELAY_TIMER_IRQn, 3);              //'???????

  TMR_DISABEL();
}

/*----------------------------------------------------------------------------------
Function   :DELAY_TIMER_IRQHandler
Description:定时器中断
Input      :无
Retrun     :无
----------------------------------------------------------------------------------*/
void DELAY_TIMER_IRQHandler(void)
{
  // timer_interrupt_flag_clear(DELAY_TIMER, TIMER_INT_FLAG_UP); //?????????
  if (timer_interrupt_flag_get(DELAY_TIMER, TIMER_INT_FLAG_UP) != RESET) //????
  {
    timer_interrupt_flag_clear(DELAY_TIMER, TIMER_INT_FLAG_UP);
  }
  us_count++;
}

/*----------------------------------------------------------------------------------
Function   :delay_us
Description:延时us
Input      :count:微秒数
Retrun     :无
----------------------------------------------------------------------------------*/
void delay_us(uint16_t count)
{
  TMR_SET_CNT(0); // timer_counter_value_config(DELAY_TIMER,0);
  timer_interrupt_enable(DELAY_TIMER, TIMER_INT_UP);
  //TMR_ENABLE(); // timer_enable
  timer_enable(DELAY_TIMER);
  us_count = 0;
  while (us_count < count)
    ;
	timer_interrupt_disable(DELAY_TIMER, TIMER_INT_UP);
  timer_disable(DELAY_TIMER);
  //TMR_DISABEL(); // timer_disable
   
}

#endif
/// @}

/// @}

/// @}
