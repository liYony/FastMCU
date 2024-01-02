/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-18     Bernard      the first version
 * 2006-04-26     Bernard      add semaphore APIs
 * 2006-08-10     Bernard      add version information
 * 2007-01-28     Bernard      rename RT_OBJECT_Class_Static to RT_Object_Class_Static
 * 2007-03-03     Bernard      clean up the definitions to rtdef.h
 * 2010-04-11     yi.qiu       add module feature
 * 2013-06-24     Bernard      add rt_kprintf re-define when not use RT_USING_CONSOLE.
 * 2016-08-09     ArdaFu       add new thread and interrupt hook.
 * 2018-11-22     Jesven       add all cpu's lock and ipi handler
 * 2021-02-28     Meco Man     add RT_KSERVICE_USING_STDLIB
 * 2021-11-14     Meco Man     add rtlegacy.h for compatibility
 * 2022-06-04     Meco Man     remove strnlen
 * 2023-05-20     Bernard      add rtatomic.h header file to included files.
 * 2023-06-30     ChuShicheng  move debug check from the rtdebug.h
 * 2023-10-16     Shell        Support a new backtrace framework
 * 2023-12-19     liYony       Adaptive FastMCU
 * 2023-12-29     liYony       add Finite state machine
 */

#ifndef __FASTMCU_H__
#define __FASTMCU_H__

#include <fmconfig.h>
#include <fmdef.h>
#include <fmservice.h>

void fmcu_setup(void);
void fmcu_loop(void);

#ifdef FM_USING_COMPONENTS_INIT
void fm_components_init(void);
void fm_components_board_init(void);
#endif /* FM_USING_COMPONENTS_INIT */

/* Clock api */
void fm_update_delay_param(void);
void fm_inc_systick(void);
fm_uint32_t fm_get_systick(void);
fm_uint64_t fm_get_systick_plus(void);
void fm_delay_ms(fm_uint16_t xms);
void fm_delay_us(fm_uint32_t xus);

/* Kservice api */
void *fm_memset(void *s, int c, fm_ubase_t count);
void *fm_memcpy(void *dst, const void *src, fm_ubase_t count);
void *fm_memmove(void *dest, const void *src, fm_size_t n);
fm_int32_t fm_memcmp(const void *cs, const void *ct, fm_size_t count);
char *fm_strstr(const char *s1, const char *s2);
fm_int32_t fm_strcasecmp(const char *a, const char *b);
fm_int32_t fm_strncmp(const char *cs, const char *ct, fm_size_t count);
fm_int32_t fm_strcmp(const char *cs, const char *ct);
char *fm_strncpy(char *dst, const char *src, fm_size_t n);
char *fm_strcpy(char *dst, const char *src);
fm_size_t fm_strlen(const char *s);
fm_size_t fm_strnlen(const char *s, fm_ubase_t maxlen);
void fm_show_version(void);
#if defined(FM_USING_DEVICE) && defined(FM_USING_CONSOLE)
fm_device_t fm_console_get_device(void);
fm_device_t fm_console_set_device(const char *name);
#endif /* defined(FM_USING_DEVICE) && defined(FM_USING_CONSOLE) */

void fm_hw_console_output(const char *str);
fm_base_t fm_hw_interrupt_disable(void);
void fm_hw_interrupt_enable(fm_base_t level);

/*
 * general kernel service
 */
#ifndef FM_USING_CONSOLE
#define fm_kprintf(...)
#define fm_kputs(str)
#else
int fm_kprintf(const char *fmt, ...);
void fm_kputs(const char *str);
#endif /* FM_USING_CONSOLE */

#ifdef FM_USING_HEAP
/*
 * heap memory interface
 */
void fm_system_heap_init(void *begin_addr, void *end_addr);

void *fm_malloc(fm_size_t size);
void fm_free(void *ptr);
void *fm_realloc(void *ptr, fm_size_t newsize);
void *fm_calloc(fm_size_t count, fm_size_t size);
void *fm_malloc_align(fm_size_t size, fm_size_t align);
void fm_free_align(void *ptr);

void fm_memory_info(fm_size_t *total,
                    fm_size_t *used,
                    fm_size_t *max_used);
#endif /* FM_USING_HEAP */

#ifdef FM_USING_MEMHEAP
/**
 * memory heap object interface
 */
fm_err_t fm_memheap_init(struct fm_memheap *memheap,
                         const char        *name,
                         void              *stafm_addr,
                         fm_size_t         size);
void *fm_memheap_alloc(struct fm_memheap *heap, fm_size_t size);
void *fm_memheap_realloc(struct fm_memheap *heap, void *ptr, fm_size_t newsize);
void fm_memheap_free(void *ptr);
void fm_memheap_info(struct fm_memheap *heap,
                     fm_size_t *total,
                     fm_size_t *used,
                     fm_size_t *max_used);
#endif /* FM_USING_MEMHEAP */

/**
 * ringbuffer api
 */
void fm_ringbuffer_init(struct fm_ringbuffer *rb, fm_uint8_t *pool, fm_int32_t size);
void fm_ringbuffer_reset(struct fm_ringbuffer *rb);
fm_size_t fm_ringbuffer_put(struct fm_ringbuffer *rb, const fm_uint8_t *ptr, fm_uint32_t length);
fm_size_t fm_ringbuffer_put_force(struct fm_ringbuffer *rb, const fm_uint8_t *ptr, fm_uint32_t length);
fm_size_t fm_ringbuffer_putchar(struct fm_ringbuffer *rb, const fm_uint8_t ch);
fm_size_t fm_ringbuffer_putchar_force(struct fm_ringbuffer *rb, const fm_uint8_t ch);
fm_size_t fm_ringbuffer_get(struct fm_ringbuffer *rb, fm_uint8_t *ptr, fm_uint32_t length);
fm_size_t fm_ringbuffer_peek(struct fm_ringbuffer *rb, fm_uint8_t **ptr);
fm_size_t fm_ringbuffer_getchar(struct fm_ringbuffer *rb, fm_uint8_t *ch);
fm_size_t fm_ringbuffer_data_len(struct fm_ringbuffer *rb);

#ifdef FM_USING_HEAP
struct fm_ringbuffer* fm_ringbuffer_create(fm_uint32_t length);
void fm_ringbuffer_destroy(struct fm_ringbuffer *rb);
#endif /* FM_USING_HEAP */

/* Completion API */
void fm_completion_init(struct fm_completion *completion);
void fm_completion_wait(struct fm_completion *completion,
                            fm_int32_t            timeout);
void fm_completion_done(struct fm_completion *completion);

/* Finite state machine API */
fsm_info_t fm_fsm_find(const char *name);
fm_err_t fm_fsm_register(fsm_info_t fsm, const char *name);
fm_err_t fm_fsm_unregister(fsm_info_t fsm);
fm_err_t fm_fsm_state_bind(fsm_info_t fsm,
                           fm_uint32_t state,
                           s_enter_fn_t fn_enter,
                           s_exit_fn_t fn_exit,
                           s_exec_fn_t fn_exec);
fm_err_t fm_fsm_state_unbind(fsm_info_t fsm,
                             fm_uint32_t state);
fm_err_t fm_fsm_event_bind(fsm_info_t fsm,
                           fm_uint32_t state,
                           fm_uint32_t event,
                           s_event_fn_t fn_event);
fm_err_t fm_fsm_event_unbind(fsm_info_t fsm,
                             fm_uint32_t state,
                             fm_uint32_t event);
fm_err_t fm_fsm_transfer(fsm_info_t fsm, fm_uint32_t state);
fm_err_t fm_fsm_invoke_event(fsm_info_t fsm, fm_uint32_t event, void *arg);
fm_uint32_t fm_fsm_current_state(fsm_info_t fsm);
void fm_fsm_execute(void);
void fm_fsm_show(void);

fm_timer_t fm_timer_find(const char *name);
fm_err_t fm_timer_init(fm_timer_t timer,
                       const char *name,
                       void (*timeout)(void *parameter),
                       void *parameter,
                       fm_uint8_t type);
fm_err_t fm_timer_detach(fm_timer_t timer);
#ifdef FM_USING_HEAP
fm_timer_t fm_timer_create(const char *name,
                           void (*timeout)(void *parameter),
                           void *parameter,
                           fm_uint8_t type);
fm_err_t fm_timer_delete(fm_timer_t timer);
#endif /* FM_USING_HEAP */
void fm_timer_loop(void);
fm_err_t fm_timer_start(fm_timer_t timer, fm_uint32_t ms);
fm_err_t fm_timer_stop(fm_timer_t timer);

/**
 * @brief Get the buffer size of the ring buffer object.
 *
 * @param rb        A pointer to the ring buffer object.
 *
 * @return  Buffer size.
 */
fm_inline fm_uint32_t fm_ringbuffer_get_size(struct fm_ringbuffer *rb)
{
    FM_ASSERT(rb != FM_NULL);
    return rb->buffer_size;
}

/** return the size of empty space in rb */
#define fm_ringbuffer_space_len(rb) ((rb)->buffer_size - fm_ringbuffer_data_len(rb))

/*
 * device (I/O) system interface
 */
fm_device_t fm_device_find(const char *name);

fm_err_t fm_device_register(fm_device_t dev,
                            const char *name,
                            fm_uint16_t flags);
fm_err_t fm_device_unregister(fm_device_t dev);

#ifdef FM_USING_HEAP
fm_device_t fm_device_create(int type, int attach_size);
void fm_device_destroy(fm_device_t device);
#endif

fm_err_t
fm_device_set_rx_indicate(fm_device_t dev,
                          fm_err_t (*rx_ind)(fm_device_t dev, fm_size_t size));
fm_err_t
fm_device_set_tx_complete(fm_device_t dev,
                          fm_err_t (*tx_done)(fm_device_t dev, void *buffer));

fm_err_t  fm_device_init (fm_device_t dev);
fm_err_t  fm_device_open (fm_device_t dev, fm_uint16_t oflag);
fm_err_t  fm_device_close(fm_device_t dev);
fm_ssize_t fm_device_read(fm_device_t dev,
                          fm_off_t    pos,
                          void       *buffer,
                          fm_size_t   size);
fm_ssize_t fm_device_write(fm_device_t dev,
                          fm_off_t    pos,
                          const void *buffer,
                          fm_size_t   size);
fm_err_t  fm_device_control(fm_device_t dev, int cmd, void *arg);

#ifdef FM_USING_DEVICE_SHOW
void fm_device_show(void);
#endif /* FM_USING_DEVICE_SHOW */

#endif /* __FASTMCU_H__ */
