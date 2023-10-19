# Quick-Kit

## 1 移植

主要填充文件为board_port.c

### 1.1 qk_hal_system_init 系统级别的一些初始化

主要包括一些系统运行前的一些准备，比如时钟的初始化等。

```c
// stm32系列示例
void qk_hal_system_init(void)
{
    HAL_Init();
    SystemClock_Config();
    if (SysTick_Config(SystemCoreClock / TICK_FRQ_HZ))
    {
        /* capture error */
        while (1)
        {
        }
    }
}
```

### 1.2 qk_hal_user_init 用户外设级别的相关初始化

主要是MCU的一些必须外设的初始化以及一些用户需要单独开启的初始化。

```c
void qk_hal_user_init()
{
    MX_USART1_UART_Init();
}
```

### 1.3 qk_hal_log_write 提供系统的log打印功能

```c
int qk_hal_log_write(const uint8_t *pbuf, uint16_t len)
{
    uart_send_nbyte((uint8_t *)pbuf, len);
    return 0;
}
```

### 1.4 SysTick_Handler 系统滴答定时器的中断

```c
void SysTick_Handler(void)
{
    qk_hal_inc_systick();
}
```

## 2 如何使用

```c
#include <qk_section.h>
#include <qk_halport.h>

int main(void)
{
    qk_hal_init();
    qk_init();
    while(1)
    {
        qk_exec();
    }
}
```

## 生成mdk工程

进入具体BSP的目录， 执行 `scons`。

## 维护

[liYony](https://gitee.com/liYony)
