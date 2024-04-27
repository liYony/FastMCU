# pwm

## 1. 示例代码

```c
#include <fmcu.h>
#include <fmdevice.h>

#define PWM_DEV_NAME        "pwm_a2"
#define PWM_DEV_CHANNEL     4

int main(void)
{
    fmcu_setup(); 
    fm_kprintf("FastMCU app running.\r\n");

    fm_uint32_t period = 500000;
    fm_uint32_t pulse = 200000;

    struct fm_device_pwm *pwm_dev = (struct fm_device_pwm *)fm_device_find(PWM_DEV_NAME);
    fm_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
    fm_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);

    while(1)
    {
    }
}
```