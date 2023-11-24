#include <fm_mem.h>

#define DBG_LVL DBG_LOG
#include <fm_log.h>

#if defined(MEM_ENABLE)

#pragma pack(1)
typedef struct head
{
    uint32_t     status : 8;
    uint32_t     size : 24;
    struct head *next;
} fm_mem_unit_head_t;
#pragma pack()

#define MEM_SIZE (((MEM_BUF_SIZE) >> 2) << 2)

#define MEM_UNIT_USED (0xAA)
#define MEM_UNIT_FREE (0x55)

#if defined(__CC_ARM)
__attribute__((aligned(4))) static uint8_t fm_mem_buf[MEM_SIZE];
#elif defined(__ICCARM__)
#pragma data_alignment = 4
static uint8_t fm_mem_buf[MEM_SIZE];
#elif defined(__GNUC__)
__attribute__((aligned(4))) static uint8_t fm_mem_buf[MEM_SIZE];
#endif

static uint8_t fm_mem_init_flag = 0;

static void _fm_mem_init()
{
    fm_mem_unit_head_t *phead = (fm_mem_unit_head_t *)fm_mem_buf;
    if (fm_mem_init_flag == 0)
    {
        memset(fm_mem_buf, 0, sizeof(fm_mem_buf));
        phead->status = MEM_UNIT_FREE;
        phead->size   = MEM_SIZE - sizeof(fm_mem_unit_head_t);
        phead->next   = NULL;
        fm_mem_init_flag = 1;
    }
}

static void *_fm_mem_alloc(uint32_t size)
{
    size = FM_ALIGN(size, 4);

    fm_mem_unit_head_t *phead      = (fm_mem_unit_head_t *)fm_mem_buf;
    void            *pret       = NULL;
    uint32_t         real_size  = size + sizeof(fm_mem_unit_head_t);
    fm_mem_unit_head_t *p_new_head = NULL;
    while (phead)
    {
        if (phead->status == MEM_UNIT_USED)
        {
            phead = phead->next;
        }
        else
        {
            if (phead->size == size)
            {
                phead->status = MEM_UNIT_USED;
                pret          = (void *)(((char *)phead) + sizeof(fm_mem_unit_head_t));
            }
            else if (phead->size < real_size)
            {
                phead = phead->next;
            }
            else
            {
                phead->status = MEM_UNIT_USED;
                pret          = (void *)(((char *)phead) + sizeof(fm_mem_unit_head_t));
                // create a new unit
                p_new_head         = (fm_mem_unit_head_t *)(((char *)pret) + size);
                p_new_head->status = MEM_UNIT_FREE;
                p_new_head->size   = phead->size - size - sizeof(fm_mem_unit_head_t);
                p_new_head->next   = phead->next;
                phead->size        = size;
                phead->next        = p_new_head;
            }
        }
        if (pret != NULL)
        {
            break;
        }
    }
    return pret;
}

static void _fm_mem_free(uint32_t addr)
{
    fm_mem_unit_head_t *phead = (fm_mem_unit_head_t *)(addr - sizeof(fm_mem_unit_head_t));
    if (phead->status != MEM_UNIT_USED ||
        addr < (((uint32_t)fm_mem_buf) + sizeof(fm_mem_unit_head_t)) ||
        addr > (((uint32_t)fm_mem_buf) + MEM_SIZE - sizeof(fm_mem_unit_head_t)))
    {
        return;
    }
    phead->status = MEM_UNIT_FREE;
    phead         = (fm_mem_unit_head_t *)fm_mem_buf;

    while (phead != NULL)
    {
        if (phead->status == MEM_UNIT_FREE)
        {
            while (phead->next != NULL && phead->next->status == MEM_UNIT_FREE)
            {
                phead->size += phead->next->size + sizeof(fm_mem_unit_head_t);
                phead->next = phead->next->next;
            }
        }
        phead = phead->next;
    }
}

static uint32_t _fm_get_free_size()
{
    fm_mem_unit_head_t *phead = (fm_mem_unit_head_t *)fm_mem_buf;
    uint32_t         ret   = 0;
    while (phead)
    {
        if (phead->status == MEM_UNIT_FREE)
        {
            ret += phead->size;
        }
        phead = phead->next;
    }
    return ret;
}

#if defined(MEM_MONITOR_ENABLE)
static void *_fm_malloc(uint32_t size)
{
    _fm_mem_init();
    if (size == 0)
    {
        return NULL;
    }
    return _fm_mem_alloc(size);
}

static void _fm_free(void *paddr)
{
    if (paddr == NULL || fm_mem_init_flag == 0)
    {
        return;
    }
    _fm_mem_free((uint32_t)paddr);
}
#else
void *fm_malloc(uint32_t size)
{
    _fm_mem_init();
    if (size == 0)
    {
        return NULL;
    }
    return _fm_mem_alloc(size);
}

void fm_free(void *paddr)
{
    if (paddr == NULL || fm_mem_init_flag == 0)
    {
        return;
    }
    _fm_mem_free((uint32_t)paddr);
}
#endif /* MEM_MONITOR_ENABLE */

uint32_t fm_get_free_size()
{
    _fm_mem_init();
    return _fm_get_free_size();
}

#if defined(MEM_MONITOR_ENABLE)

void *fm_malloc_plus(uint32_t size, const char *func, int line)
{
    void *ptr = _fm_malloc(size);
    log_i("malloc in %s, %d, size %d, address %p\r\n", func, line, size, ptr);
    return ptr;
}

void fm_free_plus(void *ptr, const char *func, int line)
{
    _fm_free(ptr);
    log_i("free in %s, %d, address %p\r\n", func, line, ptr);
}

#endif /* MEM_MONITOR_ENABLE */

void *fm_calloc(uint32_t count, uint32_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = fm_malloc(count * size);
    /* zero the memory */
    if (p)
    {
        memset(p, 0, count * size);
    }
    return p;
}

void *fm_realloc(void *ptr, uint32_t newsize)
{
    return NULL;
}

#endif /* MEM_ENABLE */
