#ifndef __DAL_GPIO_H__
#define __DAL_GPIO_H__

#include <dal_type.h>

typedef enum
{
    DAL_LVL_LOW,
    DAL_LVL_HIGH,
    DAL_LVL_INVALID,
} dal_gpio_level_t;

typedef enum
{
    DAL_GPIOA,
    DAL_GPIOB,
    DAL_GPIOC,
    DAL_GPIOD,
    DAL_GPIOE,
    DAL_GPIOF,
    DAL_GPIOG,
    DAL_GPIO_INVALID,
} dal_gpio_port_t;

typedef enum
{
    DAL_PIN0,
    DAL_PIN1,
    DAL_PIN2,
    DAL_PIN3,
    DAL_PIN4,
    DAL_PIN5,
    DAL_PIN6,
    DAL_PIN7,
    DAL_PIN8,
    DAL_PIN9,
    DAL_PIN10,
    DAL_PIN11,
    DAL_PIN12,
    DAL_PIN13,
    DAL_PIN14,
    DAL_PIN15,
    DAL_PINAll,
    DAL_PIN_INVALID,
} dal_gpio_pin_t;

typedef enum
{
    DAL_GPIO_INPUT,
    DAL_GPIO_OUTPUT,
    DAL_GPIO_DIR_INVALID,
} dal_gpio_dir_t;

typedef enum
{
    DAL_GPIO_NOPULL,
    DAL_GPIO_PULLUP,
    DAL_GPIO_PULLDOWN,
    DAL_GPIO_PULL_INVALID,
} dal_gpio_pull_t;

typedef struct
{
    dal_gpio_port_t port;
    dal_gpio_pin_t  pin;
} dal_gpio_instance_t;

typedef enum
{
    DAL_GPIO_EXTI0,
    DAL_GPIO_EXTI1,
    DAL_GPIO_EXTI2,
    DAL_GPIO_EXTI3,
    DAL_GPIO_EXTI4,
    DAL_GPIO_EXTI5,
    DAL_GPIO_EXTI6,
    DAL_GPIO_EXTI7,
    DAL_GPIO_EXTI8,
    DAL_GPIO_EXTI9,
    DAL_GPIO_EXTI10,
    DAL_GPIO_EXTI11,
    DAL_GPIO_EXTI12,
    DAL_GPIO_EXTI13,
    DAL_GPIO_EXTI14,
    DAL_GPIO_EXTI15,
    DAL_GPIO_EXIT_LINE_INVALID
} dal_gpio_exitline_t;

typedef enum
{
    DAL_GPIO_EXTI_RISE,
    DAL_GPIO_EXTI_FALL,
    DAL_GPIO_EXTI_BOTH,
    DAL_GPIO_EXTI_HIGH,
    DAL_GPIO_EXTI_LOW,
    DAL_GPIO_EXIT_TRIG_INVALID
} dal_gpio_exittrig_t;

void mcu_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull);
void mcu_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl);
dal_gpio_level_t mcu_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin);
void dal_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull);
void dal_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl);
dal_gpio_level_t dal_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin);


#endif
