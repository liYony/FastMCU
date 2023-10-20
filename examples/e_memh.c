#include <qk_memh.h>
#include <qk_section.h>

#define LOG_LVL LVL_DEBUG
#include <qk_log.h>

uint8_t *malloc_buffer_1 = NULL;
uint8_t *malloc_buffer_2 = NULL;

void example_memh(void)
{
    log_d("memh free size %d\r\n", qk_get_free_size());
    malloc_buffer_1 = qk_malloc(200);
    log_d("malloc buffer 1<0x%08x>, free size %d\r\n", malloc_buffer_1, qk_get_free_size());
    qk_free(malloc_buffer_1);
    log_d("free buffer 1, free size %d\r\n", qk_get_free_size());
    
    malloc_buffer_1 = qk_malloc(200);
    malloc_buffer_2 = qk_malloc(400);
    log_d("malloc buffer 1<0x%08x>/2<0x%08x>, free size %d\r\n", malloc_buffer_1, malloc_buffer_2, qk_get_free_size());
    qk_free(malloc_buffer_1);
    malloc_buffer_1 = qk_malloc(199);
    log_d("malloc buffer 1<0x%08x>, free size %d\r\n", malloc_buffer_1, qk_get_free_size());
    qk_free(malloc_buffer_1);
    malloc_buffer_1 = qk_malloc(201);
    log_d("malloc buffer 1<0x%08x>, free size %d\r\n", malloc_buffer_1, qk_get_free_size());
}

INITLV4_EXPORT(example_memh);
