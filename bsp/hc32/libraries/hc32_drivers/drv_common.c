#include "drv_common.h"
#include "board.h"

#define DBG_TAG    "drv_common"
#define DBG_LVL    DBG_INFO
#include <fmdbg.h>

/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE               (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                                         LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP               (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)


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

static int cm_uart3_init(uint32_t baudrate)
{
    stc_usart_uart_init_t stcUartInit;
    
    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_10, GPIO_FUNC_33);// RX
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_11, GPIO_FUNC_32);// TX
    /* Enable peripheral clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART3, ENABLE);
    /* Initialize UART. */
    USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32Baudrate = baudrate;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if(LL_OK != USART_UART_Init(CM_USART3, &stcUartInit, NULL))
    {
        return -1;
    }
    /* Enable RX function */
    USART_FuncCmd(CM_USART3, USART_RX, ENABLE);
    USART_FuncCmd(CM_USART3, USART_TX, ENABLE);
    
    return 0;
}

static void UartSendByte(uint8_t dat)
{
    /* If  don't, some data will be garbled and either start byte or terminate byte */
    while(RESET == USART_GetStatus(CM_USART3, USART_FLAG_TX_EMPTY));
    USART_WriteData(CM_USART3, (uint16_t)dat);
    /*UsartTxComplete is must !!! */
    while(RESET == USART_GetStatus(CM_USART3, USART_FLAG_TX_CPLT));
}

static void cm_uart3_send_nbyte(uint8_t *buf, uint16_t len)
{
    //  USART_FuncCmd(USART_CH, USART_RX, ENABLE);
    while(len--)
        UartSendByte(*buf++);
    //    USART_FuncCmd(USART_CH, UsartTx, DISABLE);
}

uint8_t heap_buffer[4096];

fm_weak void fm_hw_board_init(void)
{
    /* Peripheral registers write unprotected */
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);

    /* System clock initialization */
    SystemClock_Config();
    PeripheralClock_Config();
    /* Configure the SysTick */
    SysTick_Configuration();
    
    cm_uart3_init(115200);

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
    cm_uart3_send_nbyte((uint8_t *)str, fm_strlen(str));
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
