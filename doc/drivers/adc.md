# adc

## 1. 示例代码

```c
#include <fmcu.h>
#include <fmdevice.h>
                                                                                       
#define ADC_DEV_NAME        "adc3"
#define ADC_DEV_CHANNEL     12

int main(void)
{
    fmcu_setup(); 
    fm_kprintf("FastMCU app running.\r\n");

    fm_adc_device_t adc_device = (fm_adc_device_t)fm_device_find(ADC_DEV_NAME);
    while(1)
    {
        fm_adc_enable(adc_device, ADC_DEV_CHANNEL);
        fm_kprintf("read adc value = %d\n", fm_adc_read(adc_device, ADC_DEV_CHANNEL));
        fm_adc_disable(adc_device, ADC_DEV_CHANNEL);
        fm_delay_ms(1000);
    }
}
```