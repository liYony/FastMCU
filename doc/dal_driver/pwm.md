# PWM 驱动介绍

## 1 驱动接口

| **函数** | **描述**                      |
| ----------------- | ------------------- |
| dal_pwm_init()       | 初始化 PWM 设备    |
| dal_pwm_enable()     | 使能 PWM 设备      |
| dal_pwm_disable()    | 失能 PWM 设备      |
| dal_pwm_set_period() | 设置 PWM 设备的周期 |
| dal_pwm_get_period() | 获取 PWM 设备的周期 |
| dal_pwm_set_pulse()  | 设置 PWM 设备的脉冲宽度 |
| dal_pwm_get_pulse()  | 获取 PWM 设备的脉冲宽度 |

### 1.1 初始化 PWM 设备

```c
int dal_pwm_init(dal_pwm_number_t pwm, uint32_t period);
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| period   | PWM 周期时间 (单位纳秒 ns) |
| **返回** | ——                                 |
|  -1     | 失败            |
|   0     | 成功            |

### 1.2 使能 PWM 设备

```c
int dal_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| ch       | PWM 通道，详细信息查看 `dal_pwm_channel_t` 定义 |
| **返回** | ——                                 |
|  -1     | 失败            |
|   0     | 成功            |

### 1.3 失能 PWM 设备

```c
int dal_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| ch       | PWM 通道，详细信息查看 `dal_pwm_channel_t` 定义 |
| **返回** | ——                                 |
|  -1     | 失败            |
|   0     | 成功            |


### 1.4 设置 PWM 设备的周期

```c
int dal_pwm_set_period(dal_pwm_number_t pwm, uint32_t period)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| period   | PWM 周期时间 (单位纳秒 ns) |
| **返回** | ——                                 |
|  -1     | 失败            |
|   0     | 成功            |

### 1.5 获取 PWM 设备的周期

```c
uint32_t dal_pwm_get_period(dal_pwm_number_t pwm)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| **返回** | ——                                 |
|  -1     | 失败            |
| period  | PWM 周期时间 (单位纳秒 ns)            |

### 1.6 设置 PWM 设备的脉冲宽度

```c
int dal_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| ch       | PWM 通道，详细信息查看 `dal_pwm_channel_t` 定义 |
| pulse    | PWM 脉冲宽度 (单位纳秒 ns) |
| **返回** | ——                                 |
|  -1     | 失败            |
|   0     | 成功            |


### 1.7 获取 PWM 设备的脉冲宽度

```c
uint32_t dal_pwm_get_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
```

| **参数** | **描述**                           |
| -------- | ---------------------------------- |
| pwm      | PWM 设备描述符，详细信息查看 `dal_pwm_number_t` 定义 |
| ch       | PWM 通道，详细信息查看 `dal_pwm_channel_t` 定义 |
| **返回** | ——                                 |
|  -1     | 失败            |
| pulse  | PWM 脉冲宽度 (单位纳秒 ns)            |

## 2 示例代码

```c
#include <fm_section.h>

void pwm_test(void)
{
    uint32_t period, pulse;

    dal_pwm_init(DAL_PWM_0, 4000);
    dal_pwm_set_pulse(DAL_PWM_0, DAL_PWM_CH4, 2000);
    period = mcu_pwm_get_period(DAL_PWM_0);
    pulse = mcu_pwm_get_pulse(DAL_PWM_0, DAL_PWM_CH4);
    log_d("DAL_PWM_%d : period = %d, pulse = %d", DAL_PWM_0, period, pulse);
    
    dal_pwm_set_period(DAL_PWM_0, 40000);
    dal_pwm_set_pulse(DAL_PWM_0, DAL_PWM_CH4, 20000);
    period = mcu_pwm_get_period(DAL_PWM_0);
    pulse = mcu_pwm_get_pulse(DAL_PWM_0, DAL_PWM_CH4);
    log_d("DAL_PWM_%d : period = %d, pulse = %d", DAL_PWM_0, period, pulse);
    dal_pwm_enable(DAL_PWM_0, DAL_PWM_CH4);
}

INITLV4_EXPORT(pwm_test);
```
