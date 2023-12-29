# fsm

## 1 示例代码

```c
#include <fmcu.h>
#include <fmdevice.h>

static struct fsm_info _fsm;

void event1_fn(fm_uint32_t event, void *arg)
{
    fm_kprintf("%s\r\n", __func__);
    fm_fsm_transfer(&_fsm, 2);
}

void event2_fn(fm_uint32_t event, void *arg)
{
    fm_kprintf("%s\r\n", __func__);
    fm_fsm_transfer(&_fsm, 1);
}

void state1_enter_fn(fm_uint32_t pre_state)
{
    fm_kprintf("%s %d\r\n", __func__, pre_state);
}

void state1_exit_fn(void)
{
    fm_kprintf("%s\r\n", __func__);
}

void state1_exec_fn(void)
{
    fm_kprintf("%s\r\n", __func__);
}

void state2_enter_fn(fm_uint32_t pre_state)
{
    fm_kprintf("%s %d\r\n", __func__, pre_state);
}

void state2_exit_fn(void)
{
    fm_kprintf("%s\r\n", __func__);
}

void state2_exec_fn(void)
{
    fm_kprintf("%s\r\n", __func__);
}

int main(void)
{
    fmcu_startup(); 
    fm_kprintf("FastMCU app running.\r\n");
    
    fm_fsm_register(&_fsm, "fsm");
    fm_fsm_state_bind(&_fsm, 1, state1_enter_fn, state1_exit_fn, state1_exec_fn);
    fm_fsm_state_bind(&_fsm, 2, state2_enter_fn, state2_exit_fn, state2_exec_fn);
    fm_fsm_event_bind(&_fsm, 1, 1, event1_fn);
    fm_fsm_event_bind(&_fsm, 2, 2, event2_fn);
    
    fm_fsm_transfer(&_fsm, 2);
    while(1)
    {
        fm_fsm_execute();
        fm_delay_ms(1000);
        fm_fsm_invoke_event(&_fsm, 1, FM_NULL);
        fm_fsm_invoke_event(&_fsm, 2, FM_NULL);
    }
}
```