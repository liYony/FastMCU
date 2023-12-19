# serial

## 1. 示例代码

```c
#include <fmcu.h>
#include <fmdevice.h>

#define SAMPLE_UART_NAME       "uart3"
static fm_device_t serial;

int main(void)
{
    fmcu_startup(); 
    fm_kprintf("FastMCU app running.\r\n");
    
    char ch;
    char str[] = "hello FastMCU!\r\n";
    serial = fm_device_find(SAMPLE_UART_NAME);
    fm_device_open(serial, FM_DEVICE_FLAG_RX_NON_BLOCKING | FM_DEVICE_FLAG_TX_BLOCKING);
    fm_device_write(serial, 0, str, (sizeof(str) - 1));
    
    while(1)
    {
        while (fm_device_read(serial, -1, &ch, 1) != 1)
        {
        }
        fm_device_write(serial, 0, &ch, 1);
    }
}
```