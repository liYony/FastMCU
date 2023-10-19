#include <qk_urc.h>
#define LOG_LVL LVL_DEBUG
#include <qk_log.h>

static LIST_HEAD(urc_info_list_head);

static volatile void *urc_feed_data = NULL;
static volatile uint16_t urc_feed_len = 0;

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

struct urc_info* qk_urc_info_init(struct urc_info* info, const char *prefix, const char *suffix, fn_urc_cb_t cb)
{
    if (info == NULL || cb == NULL)
    {
        return NULL;
    }

    if (qk_urc_info_find(info) != NULL)
    {
        log_w("urc info <0x%08x>[%s, %s] exsit!\r\n", (uint32_t)info, prefix, suffix);
        return info;
    }
    info->cmd_prefix = prefix;
    info->cmd_suffix = suffix;
    info->callback = cb;

    list_add(&info->list, &urc_info_list_head);

    log_d("initial <0x%08x>[%s, %s] success!\r\n", (uint32_t)info, prefix, suffix);

    return info;
}

void qk_urc_info_feed(void *data, uint16_t len)
{
    urc_feed_data = data;
    urc_feed_len = len;
}

#include <qk_section.h>

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

void info1_func(const char *data, uint16_t size)
{
    log_i("%s\r\n", data);
}
struct urc_info info1;
struct urc_info info2;
struct urc_info info3;

void test_urc(void)
{
    qk_urc_info_init(&info1, "123", "234", info1_func);
    qk_urc_info_init(&info2, "111", "222", NULL);
    qk_urc_info_init(&info3, "444", "000", NULL);
    qk_urc_info_init(&info3, "333", "777", NULL);
    qk_urc_info_print();
    
    qk_urc_info_feed("123-ccc-234", 11);
}

INITLV0_EXPORT(test_urc);

void qk_urc_info_handler(void)
{
    uint8_t prefix_len, suffix_len; 
    struct list_head *pos   = NULL;
    struct urc_info     *u_info = NULL;

    if (urc_feed_data == NULL)
    {
        goto __handler_fail;
    }
    
    list_for_each(pos, &urc_info_list_head)
    {
        u_info = list_entry(pos, struct urc_info, list);
        
        prefix_len = strlen(u_info->cmd_prefix);
        suffix_len = strlen(u_info->cmd_suffix);
        if (urc_feed_len < prefix_len + suffix_len)
        {
            continue;
        }
        if ((prefix_len ? !strncmp((char *)urc_feed_data, u_info->cmd_prefix, prefix_len) : 1)
			&& (suffix_len ? !strncmp((char *)urc_feed_data + urc_feed_len - suffix_len, u_info->cmd_suffix, suffix_len) : 1))
        {
            if(u_info->callback)
			{
				memset((char *)urc_feed_data + urc_feed_len - suffix_len, 0, suffix_len);
				urc_feed_len = urc_feed_len - prefix_len - suffix_len;
				u_info->callback((char *)urc_feed_data + prefix_len, urc_feed_len);
			}
        }
    }
    
__handler_fail:
    urc_feed_data = NULL;
    urc_feed_len = 0;
}

POLLING_EXPORT(qk_urc_info_handler);
