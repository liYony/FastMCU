#ifndef __DAL_IT_H__
#define __DAL_IT_H__

#include <dal_type.h>

typedef enum
{
    DAL_EXTI_H,
    DAL_EXTI_L,
    DAL_EXTI_RISE,
    DAL_EXTI_FALL
} dal_exit_type_t;

typedef union
{
    struct
    {
        dal_exit_type_t type;
    } _exti;
    struct
    {
        uint8_t *pbuf;
        uint16_t len;
    } _uart;
} dal_it_param_t;

typedef enum
{
    DAL_HAL_IT_EXTI = 0x4954,
    DAL_HAL_IT_UART_RX,  // 串口中断接收 或 DMA接收
    DAL_HAL_IT_UART_TX,  // 串口发送中断 或 DMA 发送完成中断
    DAL_HAL_IT_TIMER_UP,
} dal_it_number_t;

typedef void (*dal_it_handler_t)(dal_it_number_t it, uint8_t index, dal_it_param_t *param,
                                void *user_data);

typedef struct
{
    dal_it_number_t   it;
    uint8_t           index;
    dal_it_handler_t  handler;
    void              *user_data;
    struct list_head  head;
} dal_it_t;

#define DAL_IT_REGISTER(name, _it, _index, _handler, _user_data) \
    static dal_it_t name;                                         \
    do                                                            \
    {                                                             \
        name.it        = _it;                                     \
        name.index     = _index;                                  \
        name.handler   = _handler;                                \
        name.user_data = _user_data;                              \
        dal_it_register(&name);                                    \
    } while (0)

/**
 * \}
 */

/**
 * \defgroup IT_Exported_Functions
 * \{
 */

void mcu_int_enable(void);
void mcu_int_disable(void);
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void dal_int_enable(void);
void dal_int_disable(void);

/**
 * 注册中断处理函数。
 * index : 例如，当int 为 DAL_HAL_IT_EXTI， index可以表示外部中断号
 *         例如  当int 为 DAL_HAL_IT_UART_RX，index可以表示串口号
 * 推荐使用：bHAL_IT_REGISTER(name, _interrupt, _index, _handler)
 */
int dal_it_register(dal_it_t *pit);

/**
 * MCU触发中断后，通过此函数告诉BOS有中断发生，并将数据传给BOS
 */
int dal_it_invoke(dal_it_number_t it, uint8_t index, dal_it_param_t *param);


#endif // !__DAL_IT_H__
