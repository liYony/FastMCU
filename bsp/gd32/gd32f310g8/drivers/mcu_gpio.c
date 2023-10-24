#include <dal.h>
#include "gd32f3x0.h"

void mcu_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
{
    uint32_t _port = (GPIO_BASE + 0x00000000U * port);
    uint32_t _pin = BIT(pin);
    uint32_t _dir = CTL_CLTR(dir);
    uint32_t _pull = PUD_PUPD(pull);

    rcu_periph_clock_enable(RCU_REGIDX_BIT(IDX_AHBEN, 17U + port));
    gpio_mode_set(_port, _dir, _pull, _pin);
    gpio_output_options_set(_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, _pin);
}

void mcu_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl)
{
    uint32_t _port = (GPIO_BASE + 0x00000000U * port);
    uint32_t _pin = BIT(pin);
    
    if(lvl == DAL_LVL_LOW)
	{
		gpio_bit_reset(_port, _pin);
	}
    else
    {
        gpio_bit_set(_port, _pin);
    }
}

dal_gpio_level_t mcu_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin)
{
    uint32_t _port = (GPIO_BASE + 0x00000000U * port);
    uint32_t _pin = BIT(pin);
    
    if(gpio_input_bit_get(_port, _pin) == RESET)
	{
		return DAL_LVL_LOW;
	}
    else
    {
        return DAL_LVL_HIGH;
    }
}

#include <qk_section.h>

void gpio_test(void)
{
    dal_gpio_config(DAL_GPIOA, DAL_PIN1, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);
    dal_gpio_write(DAL_GPIOA, DAL_PIN1, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOA, DAL_PIN1, DAL_LVL_LOW);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOA, DAL_PIN1, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOA, DAL_PIN1, DAL_LVL_LOW);
    dal_delay_ms(1000);
    
    dal_gpio_config(DAL_GPIOA, DAL_PIN5, DAL_GPIO_INPUT, DAL_GPIO_PULLUP);
}

INITLV4_EXPORT(gpio_test);

DAL_EXIT_CREATE_ATTR(exitpa5, DAL_GPIOA, DAL_PIN5, DAL_GPIO_EXTI_FALL, NULL);

void gpio_polling(void)
{
    if (dal_gpio_read(DAL_GPIOA, DAL_PIN5) == 0)
    {
        log_d("pad5 up\r\n");
    }
}

POLLING_EXPORT(gpio_polling);
