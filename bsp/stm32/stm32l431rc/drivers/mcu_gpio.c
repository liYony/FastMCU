#include <dal.h>
#include "stm32l4xx_hal.h"

#define __HAL_RCC_CLK_ENABLE(clk)               do { \
                                                    __IO uint32_t tmpreg; \
                                                    SET_BIT(RCC->AHB2ENR, clk); \
                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                    tmpreg = READ_BIT(RCC->AHB2ENR, clk); \
                                                    UNUSED(tmpreg); \
                                                } while(0)

void mcu_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
{
    uint32_t _port = (AHB2PERIPH_BASE + ((0x0000UL + port) * 0x0400UL));
    uint32_t _pin = (0x0001UL << pin);
    uint32_t _pull = pull;
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_CLK_ENABLE(0x1UL << port);
    
    GPIO_InitStruct.Pin = _pin;
    if (dir == DAL_GPIO_OUTPUT)
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    else
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = _pull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init((GPIO_TypeDef *)_port, &GPIO_InitStruct);
}

void mcu_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl)
{
    uint32_t _port = (AHB2PERIPH_BASE + ((0x0000UL + port) * 0x0400UL));
    uint32_t _pin = (0x0001UL << pin);
    
    if(lvl == DAL_LVL_LOW)
	{
		HAL_GPIO_WritePin((GPIO_TypeDef *)_port, _pin, GPIO_PIN_RESET);
	}
    else
    {
        HAL_GPIO_WritePin((GPIO_TypeDef *)_port, _pin, GPIO_PIN_SET);
    }
}

dal_gpio_level_t mcu_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin)
{
    uint32_t _port = (AHB2PERIPH_BASE + ((0x0000UL + port) * 0x0400UL));
    uint32_t _pin = (0x0001UL << pin);
    
    if(HAL_GPIO_ReadPin((GPIO_TypeDef *)_port, _pin) == GPIO_PIN_RESET)
	{
		return DAL_LVL_LOW;
	}
    else
    {
        return DAL_LVL_HIGH;
    }
}

int mcu_gpio_conifg_irq(dal_gpio_exit_attr_t *attr)
{
    uint32_t _port = (AHB2PERIPH_BASE + ((0x0000UL + attr->port) * 0x0400UL));
    uint32_t _pin = (0x0001UL << attr->pin);
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = _pin;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    switch ((int)attr->trig)
    {
    case DAL_GPIO_EXTI_RISE:
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        break;
    case DAL_GPIO_EXTI_FALL:
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        break;
    case DAL_GPIO_EXTI_BOTH:
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
        break;
    }
    HAL_GPIO_Init((GPIO_TypeDef *)_port, &GPIO_InitStruct);
    
    if (attr->pin < 5)
    {
        HAL_NVIC_SetPriority((IRQn_Type)(attr->pin + 6), 5, 0);
        HAL_NVIC_EnableIRQ((IRQn_Type)(attr->pin + 6));
    }
    else if (attr->pin >= 5 && attr->pin < 10)
    {
        HAL_NVIC_SetPriority((IRQn_Type)23, 5, 0);
        HAL_NVIC_EnableIRQ((IRQn_Type)23);
    }
    else
    {
        HAL_NVIC_SetPriority((IRQn_Type)40, 5, 0);
        HAL_NVIC_EnableIRQ((IRQn_Type)40);
    }
    
    return 0;
}

#include <qk_section.h>

DAL_EXIT_CREATE_ATTR(exitpa5, DAL_GPIOB, DAL_PIN3, DAL_GPIO_EXTI_FALL, NULL);

void exit_func(dal_it_number_t it, uint8_t index, dal_it_param_t *param, void *user_data)
{
    log_d("irq cb %d %d %d\r\n", it, index, param->_exti.type);
}

void gpio_test(void)
{
    // 输出功能
    dal_gpio_config(DAL_GPIOC, DAL_PIN13, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_LOW);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_LOW);
    dal_delay_ms(1000);
    
    // 输入功能
    dal_gpio_config(DAL_GPIOB, DAL_PIN2, DAL_GPIO_INPUT, DAL_GPIO_PULLUP);
    
    // 中断功能
    dal_gpio_attach_irq(&exitpa5, exit_func);
}

INITLV4_EXPORT(gpio_test);

void gpio_polling(void)
{
    if (dal_gpio_read(DAL_GPIOB, DAL_PIN2) == 0)
    {
        log_d("pad5 up\r\n");
    }
}

//POLLING_EXPORT(gpio_polling);

void EXTI3_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI15_10_IRQn 0 */

    /* USER CODE END EXTI15_10_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
    /* USER CODE BEGIN EXTI15_10_IRQn 1 */

    /* USER CODE END EXTI15_10_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_3)
	{
        dal_it_param_t p;
        p._exti.type = DAL_EXTI_FALL;
        dal_it_invoke(DAL_HAL_IT_EXTI, DAL_GPIO_EXTI3, &p);
	}
}
/* USER CODE END 1 */

