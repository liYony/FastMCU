#include <qk_memh.h>
#include <qk_type.h>

#define LOG_LVL LVL_DEBUG
#include <qk_log.h>

#if defined(MEMH_ENABLE)

#pragma pack(1)
typedef struct head
{
    uint32_t     status : 8;
    uint32_t     size : 24;
    struct head *next;
} qk_memh_unit_head_t;
#pragma pack()

#define MEMH_SIZE (((MEMH_BUF_SIZE) >> 2) << 2)

#define MEMH_UNIT_USED (0xAA)
#define MEMH_UNIT_FREE (0x55)

#if defined(__CC_ARM)
__attribute__((aligned(4))) static uint8_t qk_memh_buf[MEMH_SIZE];
#elif defined(__ICCARM__)
#pragma data_alignment = 4
static uint8_t qk_memh_buf[MEMH_SIZE];
#elif defined(__GNUC__)
__attribute__((aligned(4))) static uint8_t qk_memh_buf[MEMH_SIZE];
#endif

static uint8_t qk_memh_init_flag = 0;

static void _qk_memh_init()
{
    qk_memh_unit_head_t *phead = (qk_memh_unit_head_t *)qk_memh_buf;
    if (qk_memh_init_flag == 0)
    {
        memset(qk_memh_buf, 0, sizeof(qk_memh_buf));
        phead->status = MEMH_UNIT_FREE;
        phead->size   = MEMH_SIZE - sizeof(qk_memh_unit_head_t);
        phead->next   = NULL;
        qk_memh_init_flag = 1;
    }
}

static void *_qk_memh_alloc(uint32_t size)
{
    size = SIZE_ALIGNMENT(size, 4);

    qk_memh_unit_head_t *phead      = (qk_memh_unit_head_t *)qk_memh_buf;
    void            *pret       = NULL;
    uint32_t         real_size  = size + sizeof(qk_memh_unit_head_t);
    qk_memh_unit_head_t *p_new_head = NULL;
    while (phead)
    {
        if (phead->status == MEMH_UNIT_USED)
        {
            phead = phead->next;
        }
        else
        {
            if (phead->size == size)
            {
                phead->status = MEMH_UNIT_USED;
                pret          = (void *)(((char *)phead) + sizeof(qk_memh_unit_head_t));
            }
            else if (phead->size < real_size)
            {
                phead = phead->next;
            }
            else
            {
                phead->status = MEMH_UNIT_USED;
                pret          = (void *)(((char *)phead) + sizeof(qk_memh_unit_head_t));
                // create a new unit
                p_new_head         = (qk_memh_unit_head_t *)(((char *)pret) + size);
                p_new_head->status = MEMH_UNIT_FREE;
                p_new_head->size   = phead->size - size - sizeof(qk_memh_unit_head_t);
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

static void _qk_memh_free(uint32_t addr)
{
    qk_memh_unit_head_t *phead = (qk_memh_unit_head_t *)(addr - sizeof(qk_memh_unit_head_t));
    if (phead->status != MEMH_UNIT_USED ||
        addr < (((uint32_t)qk_memh_buf) + sizeof(qk_memh_unit_head_t)) ||
        addr > (((uint32_t)qk_memh_buf) + MEMH_SIZE - sizeof(qk_memh_unit_head_t)))
    {
        return;
    }
    phead->status = MEMH_UNIT_FREE;
    phead         = (qk_memh_unit_head_t *)qk_memh_buf;

    while (phead != NULL)
    {
        if (phead->status == MEMH_UNIT_FREE)
        {
            while (phead->next != NULL && phead->next->status == MEMH_UNIT_FREE)
            {
                phead->size += phead->next->size + sizeof(qk_memh_unit_head_t);
                phead->next = phead->next->next;
            }
        }
        phead = phead->next;
    }
}

static uint32_t _qk_get_free_size()
{
    qk_memh_unit_head_t *phead = (qk_memh_unit_head_t *)qk_memh_buf;
    uint32_t         ret   = 0;
    while (phead)
    {
        if (phead->status == MEMH_UNIT_FREE)
        {
            ret += phead->size;
        }
        phead = phead->next;
    }
    return ret;
}

#if defined(MEMH_MONITOR_ENABLE)
static void *_qk_malloc(uint32_t size)
{
    _qk_memh_init();
    if (size == 0)
    {
        return NULL;
    }
    return _qk_memh_alloc(size);
}

static void _qk_free(void *paddr)
{
    if (paddr == NULL || qk_memh_init_flag == 0)
    {
        return;
    }
    _qk_memh_free((uint32_t)paddr);
}
#else
void *qk_malloc(uint32_t size)
{
    _qk_memh_init();
    if (size == 0)
    {
        return NULL;
    }
    return _qk_memh_alloc(size);
}

void qk_free(void *paddr)
{
    if (paddr == NULL || qk_memh_init_flag == 0)
    {
        return;
    }
    _qk_memh_free((uint32_t)paddr);
}
#endif /* MEMH_MONITOR_ENABLE */

uint32_t qk_get_free_size()
{
    _qk_memh_init();
    return _qk_get_free_size();
}

__WEAKDEF void qk_malloc_failed_hook()
{
    ;
}

#if defined(MEMH_MONITOR_ENABLE)

void *qk_malloc_plus(uint32_t size, const char *func, int line)
{
    void *ptr = _qk_malloc(size);
    log_d("malloc in %s, %d, size %d, address %p\r\n", func, line, size, ptr);
    return ptr;
}

void qk_free_plus(void *ptr, const char *func, int line)
{
    _qk_free(ptr);
    log_d("free in %s, %d, address %p\r\n", func, line, ptr);
}

#endif /* MEMH_MONITOR_ENABLE */

#endif /* MEMH_ENABLE */
