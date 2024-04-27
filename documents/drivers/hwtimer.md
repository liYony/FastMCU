# hwtimer

# 1. 示例代码

```c
#define HWTIMER_DEV_NAME   "tim_a10"

#include <fmcu.h>
#include <fmdevice.h>

static fm_err_t timeout_cb(fm_device_t dev, fm_size_t size)
{
    fm_kprintf("tick is :%d !\n", fm_get_systick());

    return FM_EOK;
}

int main(void)
{
    fmcu_setup(); 
    fm_kprintf("FastMCU app running.\r\n");
    
    fm_device_t hw_device =  fm_device_find(HWTIMER_DEV_NAME);
    fm_device_open(hw_device, FM_DEVICE_OFLAG_RDWR);
    fm_device_set_rx_indicate(hw_device, timeout_cb);
    
    fm_hwtimer_mode_t  mode = HWTIMER_MODE_PERIOD;
    fm_device_control(hw_device, HWTIMER_CTRL_MODE_SET, &mode);
    
    fm_hwtimerval_t timeout_s;
    timeout_s.sec = 5;
    timeout_s.usec = 0;
    
    fm_device_write(hw_device, 0, &timeout_s, sizeof(timeout_s));
    
    while(1)
    {
        fm_device_read(hw_device, 0, &timeout_s, sizeof(timeout_s));
        fm_kprintf("Read: Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);
        
        fm_delay_ms(3000);
    }
}
```