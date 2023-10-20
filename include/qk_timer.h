#ifndef __QK_TIMER_H__
#define __QK_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <qkconfig.h>
#include <qk_list.h>
#include <stdint.h>
#include <string.h>

#if defined(TIMER_ENABLE)

typedef void *qk_timer_id_t;

typedef void (*qk_timer_func_t)(void *arg);

/// Timer type.
typedef enum
{
    TIMER_ONCE     = 0,  ///< One-shot timer.
    TIMER_PERIODIC = 1   ///< Repeating timer.
} qk_timer_type_t;

typedef struct
{
    qk_timer_func_t     func;
    void                *arg;
    uint8_t             enable;
    uint64_t            tick;
    uint64_t            cycle;
    qk_timer_type_t     type;
    struct list_head    list;
} qk_timer_attr_t;

#define TIMER_CREATE_ATTR(attr_name) static qk_timer_attr_t attr_name = {.func = NULL, .arg = NULL}

/// 创建定时器
/// \param[in]     func          定时器回调函数 \ref qk_timer_attr_t
/// \param[in]     type          定时器类型 \ref qk_timer_type_t
/// \param[in]     argument      自定义参数
/// \param[in]     attr          通过TIMER_CREATE_ATTR创建，再传入
/// \return 定时器ID
qk_timer_id_t qk_timer_init(qk_timer_func_t func, qk_timer_type_t type, void *argument, qk_timer_attr_t *attr);

/// 启动或重新启动定时器
/// \param[in]     id      定时器id \ref qk_timer_id_t
/// \param[in]     ms      定时周期 ms
int qk_timer_start(qk_timer_id_t id, uint32_t ms);

/// 停止定时器
/// \param[in]     id      定时器id \ref qk_timer_id_t
int qk_timer_stop(qk_timer_id_t id);

/// 查询定时器是否在运行
/// \param[in]     id      定时器id \ref qk_timer_id_t
/// \return 0 not running, 1 running.
uint32_t qk_timer_is_running(qk_timer_id_t id);

/// 删除一个定时器
/// \param[in]     id      定时器id \ref qk_timer_id_t
int qk_timer_delete(qk_timer_id_t id);
qk_timer_id_t qk_timer_create(qk_timer_func_t func, qk_timer_type_t type, void *argument);
int qk_timer_detach(qk_timer_id_t id);
#endif /* TIMER_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __QK_TIMER_H__ */
