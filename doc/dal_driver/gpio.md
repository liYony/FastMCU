# GPIO 驱动介绍

## 1 驱动接口

| **函数** | **描述**                      |
| ----------------- | ------------------- |
| dal_gpio_config()     | 配置 GPIO 引脚   |
| dal_gpio_write()     | 写入 GPIO 引脚高低电平      |
| dal_gpio_read()    | 读取 GPIO 引脚电平    |
| dal_gpio_attach_irq() | 绑定 GPIO 引脚中断 |

### 1.1 配置 GPIO 引脚

```c
void dal_gpio_config(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_dir_t dir, dal_gpio_pull_t pull)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| port     | GPIO 端口号，详细信息查看 `dal_gpio_port_t` 定义 |
| pin      | GPIO 引脚号，详细信息查看 `dal_gpio_pin_t` 定义 |
| dir      | GPIO 输入输出模式，详细信息查看 `dal_gpio_dir_t` 定义 |
| pull     | GPIO 上下拉，详细信息查看 `dal_gpio_pull_t` 定义 |

### 1.2 写入 GPIO 引脚高低电平

```c
void dal_gpio_write(dal_gpio_port_t port, dal_gpio_pin_t pin, dal_gpio_level_t lvl)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| port     | GPIO 端口号，详细信息查看 `dal_gpio_port_t` 定义 |
| pin      | GPIO 引脚号，详细信息查看 `dal_gpio_pin_t` 定义 |
| lvl      | GPIO 电平，详细信息查看 `dal_gpio_level_t` 定义 |

### 1.3 读取 GPIO 引脚电平

```c
dal_gpio_level_t dal_gpio_read(dal_gpio_port_t port, dal_gpio_pin_t pin)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| port     | GPIO 端口号，详细信息查看 `dal_gpio_port_t` 定义 |
| pin      | GPIO 引脚号，详细信息查看 `dal_gpio_pin_t` 定义 |
| **返回** | ——                                 |
| DAL_LVL_LOW  | 低电平            |
| DAL_LVL_HIGH | 高电平            |


### 1.4 绑定 GPIO 引脚中断

```c
void dal_gpio_attach_irq(dal_gpio_exit_attr_t *attr, dal_it_handler_t hdr)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| attr     | GPIO 中断属性，详细信息查看 `dal_gpio_exit_attr_t` 定义 |
| hdr      | GPIO 引脚触发中断回调函数 |

> 推荐 attr 变量使用 `DAL_EXIT_CREATE_ATTR` 宏进行创建。

## 2 示例代码

### 2.1 输出模式

```c
void gpio_test(void)
{
    dal_gpio_config(DAL_GPIOC, DAL_PIN13, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);

    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_LOW);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_HIGH);
    dal_delay_ms(1000);
    dal_gpio_write(DAL_GPIOC, DAL_PIN13, DAL_LVL_LOW);
    dal_delay_ms(1000);
}

INITLV4_EXPORT(gpio_test);
```

### 2.2 输入模式

```c
void gpio_test(void)
{
    dal_gpio_config(DAL_GPIOB, DAL_PIN2, DAL_GPIO_INPUT, DAL_GPIO_PULLUP);
}

INITLV4_EXPORT(gpio_test);

void gpio_polling(void)
{
    if (dal_gpio_read(DAL_GPIOB, DAL_PIN2) == 0)
    {
        log_d("gpio input.");
    }
}

POLLING_EXPORT(gpio_polling);
```

### 2.3 中断模式

```c

void exit_line3_func(dal_it_number_t it, uint8_t index, dal_it_param_t *param, void *user_data)
{
    log_d("irq cb %d %d %d\r\n", it, index, param->_exti.type);
}


DAL_EXIT_CREATE_ATTR(exit_line3, DAL_GPIOB, DAL_PIN3, DAL_GPIO_EXTI_FALL, NULL);

void gpio_test(void)
{
    dal_gpio_attach_irq(&exit_line3, exit_line3_func);
}

// 底层中断服务函数调用
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_3)
    {
        dal_it_param_t p;
        p._exti.type = DAL_EXTI_FALL;
        dal_it_invoke(DAL_HAL_IT_EXTI, DAL_GPIO_EXTI3, &p);
    }
}
```
