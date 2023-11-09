#ifndef __FM_TIMER_H__
#define __FM_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <fmconfig.h>
#include <fm_list.h>

#if defined(TIMER_ENABLE)

typedef void *fm_timer_id_t;
typedef void (*fm_timer_func_t)(void *arg);

typedef enum
{
    TIMER_ONCE     = 0,  ///< One-shot timer.
    TIMER_PERIODIC = 1   ///< Repeating timer.
} fm_timer_type_t;

typedef struct
{
    fm_timer_func_t     func;
    void                *arg;
    uint8_t             enable;
    uint64_t            tick;
    uint64_t            cycle;
    fm_timer_type_t     type;
    struct list_head    list;
} fm_timer_attr_t;

#define TIMER_CREATE_ATTR(attr_name) static fm_timer_attr_t attr_name = {.func = NULL, .arg = NULL}

#if !defined(TIMER_RUN_SECTION)
void fm_timer_core(void);
#endif /* !TIMER_RUN_SECTION */

fm_timer_id_t fm_timer_create(fm_timer_func_t func, fm_timer_type_t type, void *argument, fm_timer_attr_t *attr);
int fm_timer_delete(fm_timer_id_t id);
int fm_timer_start(fm_timer_id_t id, uint32_t ms);
int fm_timer_stop(fm_timer_id_t id);
int fm_timer_is_running(fm_timer_id_t id);

#endif /* TIMER_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __FM_TIMER_H__ */
