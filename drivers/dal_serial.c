#include <dal.h>

static LIST_HEAD(dal_uart_list_head);
static uint8_t init_s[DAL_UART_NUMBER] = {0};

static dal_uart_idle_attr_t *_dal_uart_attr_find(dal_uart_number_t uart)
{
    struct list_head   *pos   = NULL;
    dal_uart_idle_attr_t *pattr = NULL;
    list_for_each(pos, &dal_uart_list_head)
    {
        pattr = list_entry(pos, dal_uart_idle_attr_t, list);
        if (pattr->uart == uart)
        {
            return pattr;
        }
    }
    return NULL;
}

static void _dal_it_handler(dal_it_number_t it, uint8_t index, dal_it_param_t *param, void *user_data)
{
    dal_uart_idle_attr_t *pattr = (dal_uart_idle_attr_t *)user_data;
    int                 i     = 0;
    if (pattr == NULL || pattr->pbuf == NULL)
    {
        return;
    }
    for (i = 0; i < param->_uart.len; i++)
    {
        if (pattr->index < pattr->len)
        {
            pattr->pbuf[pattr->index] = param->_uart.pbuf[i];
            pattr->index += 1;
        }
    }
}

static void _dal_uart_detect_idle()
{
    struct list_head   *pos    = NULL;
    dal_uart_idle_attr_t *pattr  = NULL;
    int                 retval = -1;
    list_for_each(pos, &dal_uart_list_head)
    {
        pattr = list_entry(pos, dal_uart_idle_attr_t, list);
        if (pattr->index > 0)
        {
            if (pattr->l_index == 0)
            {
                pattr->l_index = pattr->index;
                pattr->l_tick  = dal_get_systick();
            }
            else
            {
                if (pattr->index != pattr->l_index)
                {
                    pattr->l_index = pattr->index;
                    pattr->l_tick  = dal_get_systick();
                }
                else if (dal_get_systick() - pattr->l_tick > pattr->idle_ms)
                {
                    if (pattr->callback)
                    {
                        retval = pattr->callback(pattr->pbuf, pattr->index, pattr->user_data);
                        if (retval >= 0 || pattr->index == pattr->len)
                        {
                            memset(pattr->pbuf, 0, pattr->len);
                            pattr->index = 0;
                        }
                    }
                    pattr->l_tick = dal_get_systick();
                }
            }
        }
    }
}

POLLING_EXPORT(_dal_uart_detect_idle);

dal_weak int mcu_uart_init(dal_uart_number_t uart, uint32_t band)
{
    return -1;
}

dal_weak int mcu_uart_deinit(dal_uart_number_t uart)
{
    return -1;
}

dal_weak int mcu_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len)
{
    return -1;
}

dal_weak int mcu_uart_receive(dal_uart_number_t uart, uint8_t *pbuf, uint16_t len)
{
    return -1;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

int dal_uart_init(dal_uart_number_t uart, uint32_t band)
{
    if (mcu_uart_init(uart, band) == 0)
    {
        init_s[uart] = 1;
        return 0;
    }
    
    init_s[uart] = 0;
    return -1;
}

int dal_uart_deinit(dal_uart_number_t uart)
{
    if (mcu_uart_deinit(uart) == 0)
    {
        init_s[uart] = 0;
        return 0;
    }
    
    init_s[uart] = 1;
    return -1;
}

int dal_uart_send(dal_uart_number_t uart, const uint8_t *pbuf, uint16_t len)
{
    if(init_s[uart] == 1)
    {
        return mcu_uart_send(uart, pbuf, len);
    }
    return -1;
}

int dal_uart_receive(dal_uart_number_t uart, uint8_t *pbuf, uint16_t len)
{
    if(init_s[uart] == 1)
    {
        return mcu_uart_receive(uart, pbuf, len);
    }
    return -1;
}

int dal_uart_receive_idle(dal_uart_number_t uart, dal_uart_idle_attr_t *attr)
{
    int                 retval = -1;
    dal_uart_idle_attr_t *pattr  = _dal_uart_attr_find(uart);
    if (pattr != NULL || attr == NULL || attr->pbuf == NULL)
    {
        return -1;
    }
    attr->uart         = uart;
    attr->it.it        = DAL_HAL_IT_UART_RX;
    attr->it.index     = uart;
    attr->it.handler   = _dal_it_handler;
    attr->it.user_data = attr;
    attr->index        = 0;
    attr->l_index      = 0;
    attr->l_tick       = 0;
    if (0 == (retval = dal_it_register(&attr->it)))
    {
        list_add(&attr->list, &dal_uart_list_head);
    }
    return retval;
}

