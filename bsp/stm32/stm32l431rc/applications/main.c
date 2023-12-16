#include <fmcu.h>

int main(void)
{
    fmcu_startup(); 
    fm_kprintf("FastMCU app running.\r\n");
    while(1)
    {
    }
}
