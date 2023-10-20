#include <qk_memh.h>
#include <qk_section.h>
#include <qk_timer.h>

#define LOG_LVL LVL_DEBUG
#include <qk_log.h>

TIMER_CREATE_ATTR(static_timer);

uint8_t times_dynamic = 0;
uint8_t times_static = 0;
qk_timer_id_t static_id = 0, dynamic_id = 0;
    
void example_dynamic_func(void *arg)
{
    times_dynamic ++;
    log_d("example_dynamic_func. %d times\r\n", times_dynamic);
    if (times_dynamic == 5)
    {
        qk_timer_detach(dynamic_id);
        log_d("memh size = %d\r\n", qk_get_free_size());
    }
}

void example_static_func(void *arg)
{
    times_static ++;
    log_d("example_static_func. %d times\r\n", times_static);
    if (times_static == 5)
    {
        qk_timer_delete(static_id);
        log_d("memh size = %d\r\n", qk_get_free_size());
    }
}

void example_timer(void)
{
    static_id = qk_timer_init(example_static_func, TIMER_PERIODIC, NULL, &static_timer);
    log_d("memh size = %d\r\n", qk_get_free_size());
    dynamic_id = qk_timer_create(example_dynamic_func, TIMER_PERIODIC, NULL);
    log_d("memh size = %d\r\n", qk_get_free_size());
    qk_timer_start(static_id, 1000);
    qk_timer_start(dynamic_id, 2000);
}

INITLV4_EXPORT(example_timer);
