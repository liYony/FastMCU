# pin

## 1. 示例代码

```c
#include <fmcu.h>
#include <fmdevice.h>
#include <drv_gpio.h>

#define TEST_PIN            GET_PIN(C, 11)

void test_pin_callback(void *args)
{
    fm_kprintf("test_pin_callback!\n");
}

int main(void)
{
    fmcu_startup(); 
    fm_kprintf("FastMCU app running.\r\n");
    fm_pin_mode(TEST_PIN, PIN_MODE_INPUT_PULLUP);
    fm_pin_attach_irq(TEST_PIN, PIN_IRQ_MODE_FALLING, test_pin_callback, FM_NULL);
    fm_pin_irq_enable(TEST_PIN, PIN_IRQ_ENABLE);
    while(1)
    {
    }
}
```