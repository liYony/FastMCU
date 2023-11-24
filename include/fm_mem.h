#ifndef __FM_MEM_H__
#define __FM_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fmconfig.h>
#include <fm_type.h>

#if defined(MEM_ENABLE)
#if defined(MEM_MONITOR_ENABLE)

void* fm_malloc_plus(uint32_t size, const char* func, int line);
void  fm_free_plus(void* ptr, const char* func, int line);

#define fm_malloc(size)  fm_malloc_plus((size), __func__, __LINE__)
#define fm_free(addr)    fm_free_plus((addr), __func__, __LINE__)

#else

void *fm_malloc(uint32_t size);
void  fm_free(void *paddr);

#endif /* MEM_MONITOR_ENABLE */

uint32_t fm_get_free_size(void);
void *fm_calloc(uint32_t count, uint32_t size);
void *fm_realloc(void *ptr, uint32_t newsize);

#endif /* MEM_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __FM_MEM_H__ */
