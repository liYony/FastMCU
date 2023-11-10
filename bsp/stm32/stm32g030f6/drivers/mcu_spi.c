#include <dal.h>
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"

volatile uint8_t SPI1_TxC_FLAG=0,SPI1_RxC_FLAG=0;

static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA1   ------> SPI1_SCK
  PA6   ------> SPI1_MISO
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* SPI1 DMA Init */

  /* SPI1_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMAMUX_REQ_SPI1_TX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_BYTE);

  /* SPI1_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_SPI1_RX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableNSSPulseMgt(SPI1);
  /* USER CODE BEGIN SPI1_Init 2 */
	LL_SPI_Enable(SPI1);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);//Important 2020-06-17
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);//Important 2020-06-17
  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

void SPI1_DMA_TRx_Execute(uint8_t *Txbuf, uint8_t *Rxbuf, uint8_t bufsize)
{

	
	LL_DMA_ConfigAddresses(	DMA1,
													LL_DMA_CHANNEL_2,
													(uint32_t)Txbuf,
													LL_SPI_DMA_GetRegAddr(SPI1),
													LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2));
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, bufsize);			
	//---------------------------------------------------------------	
	LL_DMA_ConfigAddresses(	DMA1, 
													LL_DMA_CHANNEL_3,
													LL_SPI_DMA_GetRegAddr(SPI1),
													//(uint32_t)&SPI_RxBuffer0,
													(uint32_t)Rxbuf,
													LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3));
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, bufsize);		
	//---------------------------------------------------------------
	//for(u8Cnt=0;u8Cnt<50;u8Cnt++)__NOP;
		
	/*
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);		
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);	
	LL_SPI_EnableDMAReq_TX(SPI1);
	LL_SPI_EnableDMAReq_RX(SPI1);
	*/
	LL_SPI_SetRxFIFOThreshold(SPI1,LL_SPI_RX_FIFO_TH_QUARTER);//Here, Important  2020-06-23
	
	LL_SPI_EnableDMAReq_RX(SPI1);
	LL_SPI_EnableDMAReq_TX(SPI1);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);		
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);		
}

void SPI1_DMA_TRx_Wait(void)
{
	while(SPI1_TxC_FLAG==0);
	SPI1_TxC_FLAG=0;
	while(SPI1_RxC_FLAG==0);
	SPI1_RxC_FLAG=0;	
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */
	if (LL_DMA_IsActiveFlag_TC2(DMA1))
	{
		SPI1_TxC_FLAG=1;
		LL_DMA_ClearFlag_GI2(DMA1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
	}
	else if (LL_DMA_IsActiveFlag_TC3(DMA1))
	{
		SPI1_RxC_FLAG=1;
		LL_DMA_ClearFlag_GI3(DMA1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);		
	}
}

int mcu_spi_writeread(dal_spi_number_t spi, const void *send_buf, void *recv_buf, int length)
{
    SPI1_DMA_TRx_Execute((uint8_t *)send_buf, (uint8_t *)recv_buf, length);
    return 0;
}

int mcu_spi_cs_ctl(dal_spi_number_t spi, uint8_t select)
{
    if (select == CS_SEL_ENABLE)
    {
        dal_gpio_write(DAL_GPIOA, DAL_PIN11, DAL_LVL_LOW);
    }
    else
    {
        dal_gpio_write(DAL_GPIOA, DAL_PIN11, DAL_LVL_HIGH);
    }
    return 0;
}
int mcu_spi_wait(dal_spi_number_t spi)
{
    SPI1_DMA_TRx_Wait();
    return 0;
}

int mcu_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    MX_DMA_Init();
    MX_SPI1_Init();
    dal_gpio_config(DAL_GPIOA, DAL_PIN11, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);
    return 0;
}
