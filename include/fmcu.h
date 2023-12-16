#ifndef __FASTMCU_H__
#define __FASTMCU_H__

#include <fmconfig.h>
#include <fmdef.h>
#include <fmservice.h>

void fmcu_startup(void);

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
fm_int32_t fm_strncmp(const char *cs, const char *ct, fm_size_t count);
char *fm_strncpy(char *dst, const char *src, fm_size_t n);
char *fm_strcpy(char *dst, const char *src);
fm_size_t fm_strlen(const char *s);
void fm_show_version(void);
#if defined(FM_USING_DEVICE) && defined(FM_USING_CONSOLE)
fm_device_t fm_console_get_device(void);
fm_device_t fm_console_set_device(const char *name);
#endif /* defined(FM_USING_DEVICE) && defined(FM_USING_CONSOLE) */
void fm_hw_console_output(const char *str);

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
#endif /* RT_USING_HEAP */

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

#endif /* __FASTMCU_H__ */
