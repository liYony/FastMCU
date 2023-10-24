#include "stm32l4xx_hal.h"
#include "dal.h"

UART_HandleTypeDef huart1;

void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 230400;
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

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART1)
  {
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      return;
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

		/* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

void uart_send_nbyte(uint8_t *bf, uint16_t len)
{
    HAL_UART_Transmit(&huart1, bf, len, 100);
}

uint8_t uart1_r_data;

int dal_uart1_idle_cb(uint8_t *pbuf, uint16_t len, void *user_data)
{
    qk_kprintf("\r\n");
    uart_send_nbyte(pbuf, len);
    qk_kprintf("\r\n");
    return 0;
}

DAL_UART_CREATE_ATTR(uart1, 256, 10, dal_uart1_idle_cb, NULL);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1) // ???????????? USART1
  {
      dal_it_param_t p;
      p._uart.pbuf = &uart1_r_data;
      p._uart.len = 1;
      dal_it_invoke(DAL_HAL_IT_UART_RX, DAL_UART_1, &p);
//    uart_send_nbyte(&uart1_r_data, 1);
    HAL_UART_Receive_IT(&huart1, &uart1_r_data, 1); // ???? UART ????
  }
}

int mcu_uart_init(dal_uart_number_t uart, uint32_t band)
{
    if (uart == DAL_UART_1)
    {
        MX_USART1_UART_Init();
        HAL_UART_Receive_IT(&huart1, &uart1_r_data, 1);
        dal_uart_receive_idle(DAL_UART_1, &uart1);
        return 0;
    }

    return -1;
}

int mcu_uart_deinit(dal_uart_number_t uart)
{
    return -1;
}

int mcu_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len)
{
    if (uart == DAL_UART_1)
    {
        uart_send_nbyte((uint8_t *)pbuf, len);
        return 0;
    }

    return -1;
}

int mcu_uart_receive(dal_uart_number_t uart, uint8_t *pbuf, uint16_t len)
{
    return -1;
}
