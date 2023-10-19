#ifndef __QK_STATE_H__
#define __QK_STATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <qkconfig.h>
#include <qk_section.h>

typedef void (*state_event_handler_t)(uint32_t event, void *arg);
typedef void (*state_enter_handler_t)(uint32_t pre_state);
typedef void (*state_exit_handler_t)(void);
typedef void (*state_handler_t)(void);

typedef struct
{
    uint32_t                event;
    state_event_handler_t   handler;
} state_event_t;

typedef struct
{
    state_event_t           *p_event_table;
    uint32_t                number;
} state_event_table_t;

typedef struct
{
    uint32_t                state;
    state_enter_handler_t   enter;
    state_exit_handler_t    exit;
    state_handler_t         handler;
    state_event_table_t     event_table;
} state_info_t;

#define STATE_REG_INSTANCE(state_info)                                                     \
    state_info_t state_info;                                                                \
    QK_SECTION_ITEM_REGISTER_FLASH(qk_states, state_info_t *, CONCAT_2(qk_, state_info)) =  \
        &state_info

int qk_state_transfer(uint32_t state);
int qk_state_invoke_event(uint32_t event, void *arg);
int qk_get_current_state(void);

#ifdef __cplusplus
}
#endif

#endif /* __QK_STATE_H__ */
