#ifndef __QK_TIMER_H__
#define __QK_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "_config/qk_config.h"
#include "utils/include/qk_list.h"
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

#if !defined(TIMER_RUN_SECTION)
/// ��ʱ��������, ��Ҫ������ѭ����
void qk_timer_core(void);
#endif /* !TIMER_RUN_SECTION */

/// ������ʱ��
/// \param[in]     func          ��ʱ���ص����� \ref qk_timer_attr_t
/// \param[in]     type          ��ʱ������ \ref qk_timer_type_t
/// \param[in]     argument      �Զ������
/// \param[in]     attr          ͨ��TIMER_CREATE_ATTR�������ٴ���
/// \return ��ʱ��ID
qk_timer_id_t qk_timer_create(qk_timer_func_t func, qk_timer_type_t type, void *argument, qk_timer_attr_t *attr);

/// ����������������ʱ��
/// \param[in]     id      ��ʱ��id \ref qk_timer_id_t
/// \param[in]     ms      ��ʱ���� ms
int qk_timer_start(qk_timer_id_t id, uint32_t ms);

/// ֹͣ��ʱ��
/// \param[in]     id      ��ʱ��id \ref qk_timer_id_t
int qk_timer_stop(qk_timer_id_t id);

/// ��ѯ��ʱ���Ƿ�������
/// \param[in]     id      ��ʱ��id \ref qk_timer_id_t
/// \return 0 not running, 1 running.
uint32_t qk_timer_is_running(qk_timer_id_t id);

/// ɾ��һ����ʱ��
/// \param[in]     id      ��ʱ��id \ref qk_timer_id_t
int qk_timer_delete(qk_timer_id_t id);

#endif /* TIMER_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __QK_TIMER_H__ */
