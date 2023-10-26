#include <dal.h>
#include "hc32_ll.h"

#define CLK_DIV    (CLK_PCLK0_DIV1 | \
                    CLK_PCLK1_DIV2 | \
                    CLK_PCLK2_DIV4 | \
                    CLK_PCLK3_DIV8 | \
                    CLK_PCLK4_DIV2 | \
                    CLK_EXCLK_DIV2 | \
                    CLK_HCLK_DIV1)

#define PERIPH_WE               (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                                 LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)

int32_t hal_xtal_clk_init(void)
{
    stc_clock_xtal_init_t stcXtalInit;
    stc_clock_pll_init_t stcPLLHInit;

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */

    CLK_SetClockDiv(CLK_BUS_CLK_ALL, CLK_DIV);


    CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8Mode   = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv    = CLK_XTAL_DRV_HIGH;//24M
    stcXtalInit.u8State  = CLK_XTAL_ON;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    CLK_XtalInit(&stcXtalInit);

    CLK_PLLStructInit(&stcPLLHInit);
    /* VCO = (24/1)*80 = 960MHz*/
    stcPLLHInit.u8PLLState = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL; //分频系数
    stcPLLHInit.PLLCFGR_f.PLLN = 40UL - 1UL;//倍频系数,24*40 = 960M
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL; //分频系数,960/4 = 240M
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL; //分频系数
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL; //分频系数
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
    int ret = CLK_PLLInit(&stcPLLHInit);
    if(ret != LL_OK)return ret;

    /* Highspeed SRAM set to 0 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);

    /* SRAM1_2_3_4_backup set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);

    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);

    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT4);

    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
    return ret;
}

/**
 *******************************************************************************
 ** \brief Initialize Clock, use HRC.
 ** \param [in] None
 ** \retval None
 ******************************************************************************/
int32_t hal_system_clk_init(void)
{
    stc_clock_pll_init_t stcPLLHInit;

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */

    CLK_SetClockDiv(CLK_BUS_CLK_ALL, CLK_DIV);

    CLK_HrcCmd(ENABLE);

    CLK_PLLStructInit(&stcPLLHInit);
    /* VCO = (16/1)*60 = 960MHz*/
    stcPLLHInit.u8PLLState = CLK_PLL_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL; //分频系数
    stcPLLHInit.PLLCFGR_f.PLLN = 60UL - 1UL;//倍频系数,12*60 = 960M
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL; //分频系数,960/4 = 240M
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL; //分频系数
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL; //分频系数
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_HRC;
    int ret = CLK_PLLInit(&stcPLLHInit);
    if(ret != LL_OK)return ret;

    /* Highspeed SRAM set to 0 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);

    /* SRAM1_2_3_4_backup set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);

    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);

    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT4);

    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);
    return ret;
}

void enable_flash_cache(void)// 开启Flash加速，否则程序运行速度远低于240Mhz
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

void dal_system_init()
{
    LL_PERIPH_WE(PERIPH_WE);
    
    int32_t ret = 0;
//    ret = hal_xtal_clk_init();
//    if(ret != LL_OK)//如果初始化外部时钟异常，改为内部时钟
//    {
    ret = hal_system_clk_init();
//    }
    while(ret != LL_OK);
    
    enable_flash_cache();
    
    SysTick_Init(TICK_FRQ_HZ);
    
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, DDL_IRQ_PRIO_00);
}

void SysTick_Handler(void)
{
    dal_inc_systick();
}

void dal_user_init()
{
    dal_uart_init(DAL_UART_3, 230400);
    dal_set_log_output(DAL_UART_3);
}
