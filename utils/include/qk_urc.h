#ifndef __QK_URC_H__
#define __QK_URC_H__

#include <qk_list.h>

typedef void (*fn_urc_cb_t)(const char *data, uint16_t size);
struct urc_info
{
    const char *cmd_prefix;
    const char *cmd_suffix;
    fn_urc_cb_t callback;
    struct list_head list;
};

#endif
