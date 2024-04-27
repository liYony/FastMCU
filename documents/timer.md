```c
#include <fmcu.h>

void timer1_timeout(void *parameter)
{
    fm_kprintf("%s\r\n", __func__);
}

void timer2_timeout(void *parameter)
{
    fm_kprintf("%s\r\n", __func__);
}

int main(void)
{
    fmcu_setup(); 
    fm_kprintf("This is hc32f4a0pi.\n");
    
    fm_timer_t timer1 = fm_timer_create("timer1", timer1_timeout, FM_NULL, FM_TIMER_FLAG_ONE_SHOT);
    fm_timer_t timer2 = fm_timer_create("timer2", timer2_timeout, FM_NULL, FM_TIMER_FLAG_PERIODIC);
    
    fm_timer_start(timer1, 1000);
    fm_timer_start(timer2, 2000);
    while(1)
    {
        fmcu_loop();
    }
}
```