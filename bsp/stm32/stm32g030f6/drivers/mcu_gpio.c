#include <dal.h>
#include "stm32g0xx_hal.h"

#define __HAL_RCC_CLK_ENABLE(clk)               do { \
                                                    __IO uint32_t tmpreg; \
                                                    SET_BIT(RCC->IOPENR, clk); \
                                                    /* Delay after an RCC peripheral clock enabling */ \
                                                    tmpreg = READ_BIT(RCC->IOPENR, clk); \
                                                    UNUSED(tmpreg); \
                                                } while(0U)

void mcu_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
{
    uint32_t _port = (IOPORT_BASE + ((0x0000UL + port) * 0x0400UL));
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
    uint32_t _port = (IOPORT_BASE + ((0x0000UL + port) * 0x0400UL));
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
    uint32_t _port = (IOPORT_BASE + ((0x0000UL + port) * 0x0400UL));
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
    uint32_t _port = (IOPORT_BASE + ((0x0000UL + attr->port) * 0x0400UL));
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
    
    if (attr->pin < 2)
    {
        HAL_NVIC_SetPriority(EXTI0_1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
    }
    else if (attr->pin >= 2 && attr->pin < 4)
    {
        HAL_NVIC_SetPriority(EXTI2_3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
    }
    else
    {
        HAL_NVIC_SetPriority(EXTI4_15_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
    }
    
    return 0;
}
