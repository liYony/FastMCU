#include <qk_urc.h>
#define LOG_LVL LVL_DEBUG
#include <qk_log.h>

static LIST_HEAD(urc_info_list_head);

struct urc_info* qk_urc_info_find(struct urc_info* info)
{
    struct list_head *pos   = NULL;
    struct urc_info     *u_info = NULL;
    list_for_each(pos, &urc_info_list_head)
    {
        u_info = list_entry(pos, struct urc_info, list);
        if (u_info == info)
        {
            break;
        }
        u_info = NULL;
    }
    return u_info;
}

void qk_urc_info_print(void)
{
    struct list_head *pos   = NULL;
    struct urc_info     *u_info = NULL;
    list_for_each(pos, &urc_info_list_head)
    {
        u_info = list_entry(pos, struct urc_info, list);
        log_i("<0x%08x>\r\n", (uint32_t)u_info);
    }
}

struct urc_info* qk_urc_info_init(struct urc_info* info, const char *prefix, const char *suffix, fn_urc_cb_t cb)
{
    if (info == NULL )// || cb == NULL)
    {
        return NULL;
    }

    if (qk_urc_info_find(info) != NULL)
    {
        return info;
    }
    info->cmd_prefix = prefix;
    info->cmd_suffix = suffix;

    list_add(&info->list, &urc_info_list_head);

    log_d("initial <0x%08x>[%s, %s] success!\r\n", (uint32_t)info, prefix, suffix);

    return info;
}

#include <qk_section.h>
fn_urc_cb_t tese;
void test_urc(void)
{
    struct urc_info info1;
    struct urc_info info2;
    struct urc_info info3;
    
    qk_urc_info_init(&info1, "123", "234", NULL);
    qk_urc_info_init(&info2, "111", "222", NULL);
    qk_urc_info_init(&info3, "444", "000", NULL);
    qk_urc_info_init(&info3, "333", "777", NULL);
    qk_urc_info_print();
}

INITLV0_EXPORT(test_urc);
