#include <dal.h>
#include "hc32_ll.h"
#include <board_config.h>

typedef struct
{
    struct hc32_irq_config irq_cfg;
    func_ptr_t cb_fn;
} gpio_info_t;

static void extint6_irq_handler(void);

gpio_info_t gpio_info[DAL_GPIO_EXIT_LINE_INVALID] = 
{
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, extint6_irq_handler},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
    {{.irq_prio = DDL_IRQ_PRIO_15}, NULL},
};

static void extint6_irq_handler(void)
{
    dal_it_param_t p;
    p._exti.type = DAL_EXTI_FALL;
    dal_it_invoke(DAL_HAL_IT_EXTI, DAL_GPIO_EXTI6, &p);
}

static void gpio_irq_config(uint8_t u8Port, uint16_t u16Pin, uint16_t u16ExInt)
{
    __IO uint16_t *PCRx;
    uint16_t pin_num;

    pin_num = __CLZ(__RBIT(u16Pin));
    PCRx = (__IO uint16_t *)((uint32_t)(&CM_GPIO->PCRA0) + ((uint32_t)u8Port * 0x40UL) + (pin_num * 4UL));
    MODIFY_REG16(*PCRx, GPIO_PCR_INTE, u16ExInt);
}

void mcu_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
{
    stc_gpio_init_t stcGpioInit;
    
    GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    
    if (dir == DAL_GPIO_OUTPUT)
    {
        stcGpioInit.u16PinDir = PIN_DIR_OUT;
        stcGpioInit.u16PinOutputType = PIN_OUT_TYPE_CMOS;
    }
    else
    {
        stcGpioInit.u16PinDir = PIN_DIR_IN;
    }
    
    if (pull == DAL_GPIO_PULLUP)
    {
        stcGpioInit.u16PullUp = PIN_PU_ON;
    }
    else if (pull == DAL_GPIO_PULLDOWN)
    {
        stcGpioInit.u16PullUp = PIN_PU_OFF;
    }
    
    GPIO_Init(port, (1 << pin), &stcGpioInit);
}

void mcu_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl)
{
    if (lvl == DAL_LVL_LOW)
    {
        GPIO_ResetPins(port, (1 << pin));
    }
    else
    {
        GPIO_SetPins(port, (1 << pin));
    }
}

dal_gpio_level_t mcu_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin)
{
    if (PIN_RESET == GPIO_ReadInputPins(port, (1 << pin)))
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
    if (gpio_info[attr->pin].cb_fn == NULL)
    {
        return -1;
    }

    stc_extint_init_t stcExtIntInit;
    
    /* Exint config */
    EXTINT_StructInit(&stcExtIntInit);
    switch ((uint8_t)attr->trig)
    {
        case (uint8_t)DAL_GPIO_EXTI_RISE:
            stcExtIntInit.u32Edge = EXTINT_TRIG_RISING;
            break;
        case (uint8_t)DAL_GPIO_EXTI_FALL:
            stcExtIntInit.u32Edge = EXTINT_TRIG_FALLING;
            break;
        case (uint8_t)DAL_GPIO_EXTI_BOTH:
            stcExtIntInit.u32Edge = EXTINT_TRIG_BOTH;
            break;
        case (uint8_t)DAL_GPIO_EXTI_LOW:
            stcExtIntInit.u32Edge = EXTINT_TRIG_LOW;
            break;
    }
    EXTINT_Init((1 << attr->pin), &stcExtIntInit);
    
    /* Gpio config */
    gpio_irq_config(attr->port, (1 << attr->pin), PIN_EXTINT_ON);
    
    /* Install irq */
    gpio_info[attr->pin].irq_cfg.int_src = (en_int_src_t)attr->pin;
    gpio_info[attr->pin].irq_cfg.irq_num =  (IRQn_Type)(GPIO_IRQN_BASE + attr->pin);
    
    hc32_install_irq_handler(&gpio_info[attr->pin].irq_cfg, gpio_info[attr->pin].cb_fn);
    
    return 0;
}

//-------------------------------------------------------------------------------------------------//

#include <fm_section.h>

#define OUT_GPIO_PORT DAL_GPIOE
#define OUT_GPIO_PIN DAL_PIN10

#define IN_GPIO_PORT DAL_GPIOB
#define IN_GPIO_PIN DAL_PIN6

DAL_EXIT_CREATE_ATTR(exit_gpio, IN_GPIO_PORT, IN_GPIO_PIN, DAL_GPIO_EXTI_FALL, NULL);

void exit_func(dal_it_number_t it, uint8_t index, dal_it_param_t *param, void *user_data)
{
    log_d("irq cb %d %d %d\r\n", it, index, param->_exti.type);
}

void gpio_test(void)
{
    // 输出功能
    dal_gpio_config(OUT_GPIO_PORT, OUT_GPIO_PIN, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);
    dal_gpio_write(OUT_GPIO_PORT, OUT_GPIO_PIN, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(OUT_GPIO_PORT, OUT_GPIO_PIN, DAL_LVL_LOW);
    dal_delay_ms(1000);
    dal_gpio_write(OUT_GPIO_PORT, OUT_GPIO_PIN, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(OUT_GPIO_PORT, OUT_GPIO_PIN, DAL_LVL_LOW);
    dal_delay_ms(1000);
    
    // 输入功能
//    dal_gpio_config(IN_GPIO_PORT, IN_GPIO_PIN, DAL_GPIO_INPUT, DAL_GPIO_PULLUP);
    
    // 中断功能
    dal_gpio_attach_irq(&exit_gpio, exit_func);
}

//INITLV4_EXPORT(gpio_test);

void gpio_polling(void)
{
    if (dal_gpio_read(IN_GPIO_PORT, IN_GPIO_PIN) == 0)
    {
        log_d("pad5 up\r\n");
    }
}

//POLLING_EXPORT(gpio_polling);
