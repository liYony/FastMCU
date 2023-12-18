/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-11-06     balanceTWK        first version
 * 2019-04-23     WillianChan       Fix GPIO serial number disorder
 * 2020-06-16     thread-liu        add STM32MP1
 * 2020-09-01     thread-liu        add GPIOZ
 * 2020-09-18     geniusgogo        optimization design pin-index algorithm
 */

#include <board.h>
#include "drv_gpio.h"

#ifdef BSP_USING_GPIO

#define PIN_NUM(port, no) (((((port)&0xFu) << 4) | ((no)&0xFu)))
#define PIN_PORT(pin) ((uint8_t)(((pin) >> 4) & 0xFu))
#define PIN_NO(pin) ((uint8_t)((pin)&0xFu))

#if defined(SOC_SERIES_STM32MP1)
#if defined(GPIOZ)
#define gpioz_pofm_base (175) /* PIN_STPOFM_MAX * 16 - 16 */
#define PIN_STPORT(pin) ((pin > gpioz_pofm_base) ? ((GPIO_TypeDef *)(GPIOZ_BASE)) : ((GPIO_TypeDef *)(GPIOA_BASE + (0x1000u * PIN_PORT(pin)))))
#else
#define PIN_STPORT(pin) ((GPIO_TypeDef *)(GPIOA_BASE + (0x1000u * PIN_PORT(pin))))
#endif /* GPIOZ */
#else
#define PIN_STPORT(pin) ((GPIO_TypeDef *)(GPIOA_BASE + (0x400u * PIN_PORT(pin))))
#endif /* SOC_SERIES_STM32MP1 */

#define PIN_STPIN(pin) ((uint16_t)(1u << PIN_NO(pin)))

#if defined(GPIOZ)
#define __STM32_POFM_MAX 12u
#elif defined(GPIOK)
#define __STM32_POFM_MAX 11u
#elif defined(GPIOJ)
#define __STM32_POFM_MAX 10u
#elif defined(GPIOI)
#define __STM32_POFM_MAX 9u
#elif defined(GPIOH)
#define __STM32_POFM_MAX 8u
#elif defined(GPIOG)
#define __STM32_POFM_MAX 7u
#elif defined(GPIOF)
#define __STM32_POFM_MAX 6u
#elif defined(GPIOE)
#define __STM32_POFM_MAX 5u
#elif defined(GPIOD)
#define __STM32_POFM_MAX 4u
#elif defined(GPIOC)
#define __STM32_POFM_MAX 3u
#elif defined(GPIOB)
#define __STM32_POFM_MAX 2u
#elif defined(GPIOA)
#define __STM32_POFM_MAX 1u
#else
#define __STM32_POFM_MAX 0u
#error Unsupported STM32 GPIO peripheral.
#endif

#define PIN_STPOFM_MAX __STM32_POFM_MAX

static const struct pin_irq_map pin_irq_map[] =
    {
#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32G0)
        {GPIO_PIN_0, EXTI0_1_IRQn},
        {GPIO_PIN_1, EXTI0_1_IRQn},
        {GPIO_PIN_2, EXTI2_3_IRQn},
        {GPIO_PIN_3, EXTI2_3_IRQn},
        {GPIO_PIN_4, EXTI4_15_IRQn},
        {GPIO_PIN_5, EXTI4_15_IRQn},
        {GPIO_PIN_6, EXTI4_15_IRQn},
        {GPIO_PIN_7, EXTI4_15_IRQn},
        {GPIO_PIN_8, EXTI4_15_IRQn},
        {GPIO_PIN_9, EXTI4_15_IRQn},
        {GPIO_PIN_10, EXTI4_15_IRQn},
        {GPIO_PIN_11, EXTI4_15_IRQn},
        {GPIO_PIN_12, EXTI4_15_IRQn},
        {GPIO_PIN_13, EXTI4_15_IRQn},
        {GPIO_PIN_14, EXTI4_15_IRQn},
        {GPIO_PIN_15, EXTI4_15_IRQn},
#elif defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32L5) || defined(SOC_SERIES_STM32U5) || defined(SOC_SERIES_STM32H5)
        {GPIO_PIN_0, EXTI0_IRQn},
        {GPIO_PIN_1, EXTI1_IRQn},
        {GPIO_PIN_2, EXTI2_IRQn},
        {GPIO_PIN_3, EXTI3_IRQn},
        {GPIO_PIN_4, EXTI4_IRQn},
        {GPIO_PIN_5, EXTI5_IRQn},
        {GPIO_PIN_6, EXTI6_IRQn},
        {GPIO_PIN_7, EXTI7_IRQn},
        {GPIO_PIN_8, EXTI8_IRQn},
        {GPIO_PIN_9, EXTI9_IRQn},
        {GPIO_PIN_10, EXTI10_IRQn},
        {GPIO_PIN_11, EXTI11_IRQn},
        {GPIO_PIN_12, EXTI12_IRQn},
        {GPIO_PIN_13, EXTI13_IRQn},
        {GPIO_PIN_14, EXTI14_IRQn},
        {GPIO_PIN_15, EXTI15_IRQn},
#elif defined(SOC_SERIES_STM32F3)
        {GPIO_PIN_0, EXTI0_IRQn},
        {GPIO_PIN_1, EXTI1_IRQn},
        {GPIO_PIN_2, EXTI2_TSC_IRQn},
        {GPIO_PIN_3, EXTI3_IRQn},
        {GPIO_PIN_4, EXTI4_IRQn},
        {GPIO_PIN_5, EXTI9_5_IRQn},
        {GPIO_PIN_6, EXTI9_5_IRQn},
        {GPIO_PIN_7, EXTI9_5_IRQn},
        {GPIO_PIN_8, EXTI9_5_IRQn},
        {GPIO_PIN_9, EXTI9_5_IRQn},
        {GPIO_PIN_10, EXTI15_10_IRQn},
        {GPIO_PIN_11, EXTI15_10_IRQn},
        {GPIO_PIN_12, EXTI15_10_IRQn},
        {GPIO_PIN_13, EXTI15_10_IRQn},
        {GPIO_PIN_14, EXTI15_10_IRQn},
        {GPIO_PIN_15, EXTI15_10_IRQn},
#else
        {GPIO_PIN_0, EXTI0_IRQn},
        {GPIO_PIN_1, EXTI1_IRQn},
        {GPIO_PIN_2, EXTI2_IRQn},
        {GPIO_PIN_3, EXTI3_IRQn},
        {GPIO_PIN_4, EXTI4_IRQn},
        {GPIO_PIN_5, EXTI9_5_IRQn},
        {GPIO_PIN_6, EXTI9_5_IRQn},
        {GPIO_PIN_7, EXTI9_5_IRQn},
        {GPIO_PIN_8, EXTI9_5_IRQn},
        {GPIO_PIN_9, EXTI9_5_IRQn},
        {GPIO_PIN_10, EXTI15_10_IRQn},
        {GPIO_PIN_11, EXTI15_10_IRQn},
        {GPIO_PIN_12, EXTI15_10_IRQn},
        {GPIO_PIN_13, EXTI15_10_IRQn},
        {GPIO_PIN_14, EXTI15_10_IRQn},
        {GPIO_PIN_15, EXTI15_10_IRQn},
#endif
};

static struct fm_pin_irq_hdr pin_irq_hdr_tab[] =
    {
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
        {-1, 0, FM_NULL, FM_NULL},
};
static uint32_t pin_irq_enable_mask = 0;

#define ITEM_NUM(items) (sizeof(items) / sizeof((items)[0]))

/* e.g. PE.7 */
static fm_base_t stm32_pin_get(const char *name)
{
    fm_base_t pin = 0;
    int hw_pofm_num, hw_pin_num = 0;
    int i, name_len;

    name_len = fm_strlen(name);

    if ((name_len < 4) || (name_len >= 6))
    {
        goto out;
    }
    if ((name[0] != 'P') || (name[2] != '.'))
    {
        goto out;
    }

    if ((name[1] >= 'A') && (name[1] <= 'Z'))
    {
        hw_pofm_num = (int)(name[1] - 'A');
    }
    else
    {
        goto out;
    }

    for (i = 3; i < name_len; i++)
    {
        hw_pin_num *= 10;
        hw_pin_num += name[i] - '0';
    }

    pin = PIN_NUM(hw_pofm_num, hw_pin_num);

    return pin;

out:
    fm_kprintf("Px.y  x:A~Z  y:0-15, e.g. PA.0\n");
    return -FM_EINVAL;
}

static void stm32_pin_write(fm_device_t dev, fm_base_t pin, fm_uint8_t value)
{
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;

    if (PIN_PORT(pin) < PIN_STPOFM_MAX)
    {
        gpio_port = PIN_STPORT(pin);
        gpio_pin = PIN_STPIN(pin);

        HAL_GPIO_WritePin(gpio_port, gpio_pin, (GPIO_PinState)value);
    }
}

static fm_int8_t stm32_pin_read(fm_device_t dev, fm_base_t pin)
{
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
    GPIO_PinState state = (GPIO_PinState)PIN_LOW;

    if (PIN_PORT(pin) < PIN_STPOFM_MAX)
    {
        gpio_port = PIN_STPORT(pin);
        gpio_pin = PIN_STPIN(pin);
        state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
    }

    return (state == GPIO_PIN_RESET) ? PIN_LOW : PIN_HIGH;
}

static void stm32_pin_mode(fm_device_t dev, fm_base_t pin, fm_uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (PIN_PORT(pin) >= PIN_STPOFM_MAX)
    {
        return;
    }

    /* Configure GPIO_InitStructure */
    GPIO_InitStruct.Pin = PIN_STPIN(pin);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    }
    else if (mode == PIN_MODE_INPUT_PULLDOWN)
    {
        /* input setting: pull down. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        /* output setting: od. */
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    }

    HAL_GPIO_Init(PIN_STPORT(pin), &GPIO_InitStruct);
}

fm_inline fm_int32_t bit2bitno(fm_uint32_t bit)
{
    fm_int32_t i;
    for (i = 0; i < 32; i++)
    {
        if (((fm_uint32_t)0x01 << i) == bit)
        {
            return i;
        }
    }
    return -1;
}

fm_inline const struct pin_irq_map *get_pin_irq_map(uint32_t pinbit)
{
    fm_int32_t mapindex = bit2bitno(pinbit);
    if (mapindex < 0 || mapindex >= (fm_int32_t)ITEM_NUM(pin_irq_map))
    {
        return FM_NULL;
    }
    return &pin_irq_map[mapindex];
};

static fm_err_t stm32_pin_attach_irq(struct fm_device *device, fm_base_t pin,
                                     fm_uint8_t mode, void (*hdr)(void *args), void *args)
{
    fm_base_t level;
    fm_int32_t irqindex = -1;

    if (PIN_PORT(pin) >= PIN_STPOFM_MAX)
    {
        return -FM_ENOSYS;
    }

    irqindex = bit2bitno(PIN_STPIN(pin));
    if (irqindex < 0 || irqindex >= (fm_int32_t)ITEM_NUM(pin_irq_map))
    {
        return -FM_ENOSYS;
    }

    level = fm_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irqindex].pin == pin &&
        pin_irq_hdr_tab[irqindex].hdr == hdr &&
        pin_irq_hdr_tab[irqindex].mode == mode &&
        pin_irq_hdr_tab[irqindex].args == args)
    {
        fm_hw_interrupt_enable(level);
        return FM_EOK;
    }
    if (pin_irq_hdr_tab[irqindex].pin != -1)
    {
        fm_hw_interrupt_enable(level);
        return -FM_EBUSY;
    }
    pin_irq_hdr_tab[irqindex].pin = pin;
    pin_irq_hdr_tab[irqindex].hdr = hdr;
    pin_irq_hdr_tab[irqindex].mode = mode;
    pin_irq_hdr_tab[irqindex].args = args;
    fm_hw_interrupt_enable(level);

    return FM_EOK;
}

static fm_err_t stm32_pin_dettach_irq(struct fm_device *device, fm_base_t pin)
{
    fm_base_t level;
    fm_int32_t irqindex = -1;

    if (PIN_PORT(pin) >= PIN_STPOFM_MAX)
    {
        return -FM_ENOSYS;
    }

    irqindex = bit2bitno(PIN_STPIN(pin));
    if (irqindex < 0 || irqindex >= (fm_int32_t)ITEM_NUM(pin_irq_map))
    {
        return -FM_ENOSYS;
    }

    level = fm_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irqindex].pin == -1)
    {
        fm_hw_interrupt_enable(level);
        return FM_EOK;
    }
    pin_irq_hdr_tab[irqindex].pin = -1;
    pin_irq_hdr_tab[irqindex].hdr = FM_NULL;
    pin_irq_hdr_tab[irqindex].mode = 0;
    pin_irq_hdr_tab[irqindex].args = FM_NULL;
    fm_hw_interrupt_enable(level);

    return FM_EOK;
}

static fm_err_t stm32_pin_irq_enable(struct fm_device *device, fm_base_t pin,
                                     fm_uint8_t enabled)
{
    const struct pin_irq_map *irqmap;
    fm_base_t level;
    fm_int32_t irqindex = -1;
    GPIO_InitTypeDef GPIO_InitStruct;

    if (PIN_PORT(pin) >= PIN_STPOFM_MAX)
    {
        return -FM_ENOSYS;
    }

    if (enabled == PIN_IRQ_ENABLE)
    {
        irqindex = bit2bitno(PIN_STPIN(pin));
        if (irqindex < 0 || irqindex >= (fm_int32_t)ITEM_NUM(pin_irq_map))
        {
            return -FM_ENOSYS;
        }

        level = fm_hw_interrupt_disable();

        if (pin_irq_hdr_tab[irqindex].pin == -1)
        {
            fm_hw_interrupt_enable(level);
            return -FM_ENOSYS;
        }

        irqmap = &pin_irq_map[irqindex];

        /* Configure GPIO_InitStructure */
        GPIO_InitStruct.Pin = PIN_STPIN(pin);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        switch (pin_irq_hdr_tab[irqindex].mode)
        {
        case PIN_IRQ_MODE_RISING:
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
            break;
        case PIN_IRQ_MODE_FALLING:
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
            break;
        case PIN_IRQ_MODE_RISING_FALLING:
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
            break;
        }
        HAL_GPIO_Init(PIN_STPORT(pin), &GPIO_InitStruct);

        HAL_NVIC_SetPriority(irqmap->irqno, 5, 0);
        HAL_NVIC_EnableIRQ(irqmap->irqno);
        pin_irq_enable_mask |= irqmap->pinbit;

        fm_hw_interrupt_enable(level);
    }
    else if (enabled == PIN_IRQ_DISABLE)
    {
        irqmap = get_pin_irq_map(PIN_STPIN(pin));
        if (irqmap == FM_NULL)
        {
            return -FM_ENOSYS;
        }

        level = fm_hw_interrupt_disable();

        HAL_GPIO_DeInit(PIN_STPORT(pin), PIN_STPIN(pin));

        pin_irq_enable_mask &= ~irqmap->pinbit;
#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32G0)
        if ((irqmap->pinbit >= GPIO_PIN_0) && (irqmap->pinbit <= GPIO_PIN_1))
        {
            if (!(pin_irq_enable_mask & (GPIO_PIN_0 | GPIO_PIN_1)))
            {
                HAL_NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else if ((irqmap->pinbit >= GPIO_PIN_2) && (irqmap->pinbit <= GPIO_PIN_3))
        {
            if (!(pin_irq_enable_mask & (GPIO_PIN_2 | GPIO_PIN_3)))
            {
                HAL_NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else if ((irqmap->pinbit >= GPIO_PIN_4) && (irqmap->pinbit <= GPIO_PIN_15))
        {
            if (!(pin_irq_enable_mask & (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                                         GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)))
            {
                HAL_NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else
        {
            HAL_NVIC_DisableIRQ(irqmap->irqno);
        }
#else
        if ((irqmap->pinbit >= GPIO_PIN_5) && (irqmap->pinbit <= GPIO_PIN_9))
        {
            if (!(pin_irq_enable_mask & (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9)))
            {
                HAL_NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else if ((irqmap->pinbit >= GPIO_PIN_10) && (irqmap->pinbit <= GPIO_PIN_15))
        {
            if (!(pin_irq_enable_mask & (GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)))
            {
                HAL_NVIC_DisableIRQ(irqmap->irqno);
            }
        }
        else
        {
            HAL_NVIC_DisableIRQ(irqmap->irqno);
        }
#endif
        fm_hw_interrupt_enable(level);
    }
    else
    {
        return -FM_ENOSYS;
    }

    return FM_EOK;
}
static const struct fm_pin_ops _stm32_pin_ops =
    {
        stm32_pin_mode,
        stm32_pin_write,
        stm32_pin_read,
        stm32_pin_attach_irq,
        stm32_pin_dettach_irq,
        stm32_pin_irq_enable,
        stm32_pin_get,
};

fm_inline void pin_irq_hdr(int irqno)
{
    if (pin_irq_hdr_tab[irqno].hdr)
    {
        pin_irq_hdr_tab[irqno].hdr(pin_irq_hdr_tab[irqno].args);
    }
}

#if defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32U5)
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
    pin_irq_hdr(bit2bitno(GPIO_Pin));
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    pin_irq_hdr(bit2bitno(GPIO_Pin));
}
#else
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    pin_irq_hdr(bit2bitno(GPIO_Pin));
}
#endif

#if defined(SOC_SERIES_STM32F0) || defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32L0)
void EXTI0_1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

#elif defined(SOC_SERIES_STM32MP1) || defined(SOC_SERIES_STM32U5)
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

void EXTI6_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
}

void EXTI7_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}

void EXTI8_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void EXTI9_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
}

void EXTI11_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void EXTI12_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}

void EXTI13_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void EXTI14_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
}

void EXTI15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

#else

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}
#endif

int fm_hw_pin_init(void)
{
#if defined(__HAL_RCC_GPIOA_CLK_ENABLE)
    __HAL_RCC_GPIOA_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOB_CLK_ENABLE)
    __HAL_RCC_GPIOB_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOC_CLK_ENABLE)
    __HAL_RCC_GPIOC_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOD_CLK_ENABLE)
    __HAL_RCC_GPIOD_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOE_CLK_ENABLE)
    __HAL_RCC_GPIOE_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOF_CLK_ENABLE)
    __HAL_RCC_GPIOF_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOG_CLK_ENABLE)
#ifdef SOC_SERIES_STM32L4
    HAL_PWREx_EnableVddIO2();
#endif
    __HAL_RCC_GPIOG_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOH_CLK_ENABLE)
    __HAL_RCC_GPIOH_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOI_CLK_ENABLE)
    __HAL_RCC_GPIOI_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOJ_CLK_ENABLE)
    __HAL_RCC_GPIOJ_CLK_ENABLE();
#endif

#if defined(__HAL_RCC_GPIOK_CLK_ENABLE)
    __HAL_RCC_GPIOK_CLK_ENABLE();
#endif

    return fm_device_pin_register("pin", &_stm32_pin_ops, FM_NULL);
}

#endif /* BSP_USING_GPIO */
