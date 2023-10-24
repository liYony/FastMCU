#include <dal_gpio.h>

dal_weak void mcu_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
{
    ;
}

dal_weak void mcu_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl)
{
    ;
}

dal_weak dal_gpio_level_t mcu_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin)
{
    return DAL_LVL_LOW;
}

void dal_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
{
    mcu_gpio_config(port, pin, dir, pull);
}

void dal_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl)
{
    mcu_gpio_write(port, pin, lvl);
}

dal_gpio_level_t dal_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin)
{
    return mcu_gpio_read(port, pin);
}
