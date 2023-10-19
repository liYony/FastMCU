#ifndef __QK_MEMH_H__
#define __QK_MEMH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <qkconfig.h>
#include <stdint.h>

#if defined(MEMH_ENABLE)
#if defined(MEMH_MONITOR_ENABLE)

void* qk_malloc_plus(uint32_t size, const char* func, int line);
void  qk_free_plus(void* ptr, const char* func, int line);

#define qk_malloc(size)  qk_malloc_plus((size), __func__, __LINE__)
#define qk_free(addr)    qk_free_plus((addr), __func__, __LINE__)

#else

void *qk_malloc(uint32_t size);
void  qk_free(void *paddr);

#endif /* MEMH_MONITOR_ENABLE */

uint32_t qk_get_free_size(void);
void     qk_malloc_failed_hook(void);

#endif /* MEMH_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __QK_MEMH_H__ */
