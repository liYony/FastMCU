/*******************************************************************************
*                                 AMetal
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 堆管理器
 *
 * \internal
 * \par modification history:
 * - 1.00 16-10-27  tee, copy from amorks
 * - 23-11-24 liYony, adapt fastmcu
 * \endinternal
 */

#include <fm_mem.h>
#include <fm_memheap.h>
#include <string.h>
#include <fm_section.h>

#define DBG_LVL DBG_LOG
#include <fm_log.h>

/* dynamic pool magic and mask */
#define __MEMHEAP_MAGIC        0x1ea01ea0
#define __MEMHEAP_MASK         0xfffffffe
#define __MEMHEAP_USED         0x01
#define __MEMHEAP_FREED        0x00

#define __MEMHEAP_IS_USED(i)   ((i)->magic & __MEMHEAP_USED)

#define __MEMHEAP_MINALLOC      12

#define __MEM_ALIGN_SIZE        (sizeof(void *))
#define __MEMHEAP_SIZE          FM_ALIGN(sizeof(struct am_memheap_item), __MEM_ALIGN_SIZE)
#define __MEMITEM_SIZE(item)    ((uint32_t)item->next - (uint32_t)item - __MEMHEAP_SIZE)


void am_memheap_free(void *ptr);

/*
 * The initialized memory pool will be:
 * +-----------------------------------+--------------------------+
 * | whole freed memory block          | Used Memory Block Tailer |
 * +-----------------------------------+--------------------------+
 *
 * block_list --> whole freed memory block
 *
 * The length of Used Memory Block Tailer is 0,
 * which is prevents block merging across list
 */
int am_memheap_init(struct am_memheap *memheap,
                         const char        *name,
                         void              *start_addr,
                         uint32_t           size)
{
    struct am_memheap_item *item;

    FM_ASSERT(memheap != NULL);

    /* initialize pool object */
    memheap->name           = name;
    memheap->start_addr     = start_addr;
    memheap->pool_size      = FM_ALIGN_DOWN(size, __MEM_ALIGN_SIZE);
    memheap->available_size = memheap->pool_size - (2 * __MEMHEAP_SIZE);
    memheap->max_used_size  = memheap->pool_size - memheap->available_size;

    /* initialize the free list header */
    item            = &(memheap->free_header);
    item->magic     = __MEMHEAP_MAGIC;
    item->pool_ptr  = memheap;
    item->next      = NULL;
    item->prev      = NULL;
    item->next_free = item;
    item->prev_free = item;

    /* set the free list to free list header */
    memheap->free_list = item;

    /* initialize the first big memory block */
    item            = (struct am_memheap_item *)start_addr;
    item->magic     = __MEMHEAP_MAGIC;
    item->pool_ptr  = memheap;
    item->next      = NULL;
    item->prev      = NULL;
    item->next_free = item;
    item->prev_free = item;

    item->next = (struct am_memheap_item *)
        ((uint8_t *)item + memheap->available_size + __MEMHEAP_SIZE);
    item->prev = item->next;

    /* block list header */
    memheap->block_list = item;

    /* place the big memory block to free list */
    item->next_free = memheap->free_list->next_free;
    item->prev_free = memheap->free_list;
    memheap->free_list->next_free->prev_free = item;
    memheap->free_list->next_free            = item;

    /* move to the end of memory pool to build a small tailer block,
     * which prevents block merging
     */
    item = item->next;
    /* it's a used memory block */
    item->magic     = __MEMHEAP_MAGIC | __MEMHEAP_USED;
    item->pool_ptr  = memheap;
    item->next      = (struct am_memheap_item *)start_addr;
    item->prev      = (struct am_memheap_item *)start_addr;
    /* not in free list */
    item->next_free = item->prev_free = NULL;

    log_d("create memory heap %X (start 0x%08X, size %d)\n",
              memheap, start_addr, size);

    return FM_EOK;
}

/******************************************************************************/
void *am_memheap_alloc(struct am_memheap *heap, uint32_t size)
{
    uint32_t free_size;
    struct am_memheap_item *header_ptr;

    FM_ASSERT(heap != NULL);

    /* align allocated size */
    size = FM_ALIGN(size, __MEM_ALIGN_SIZE);
    if (size < __MEMHEAP_MINALLOC) {
        size = __MEMHEAP_MINALLOC;
    }

    log_d("allocate %d on heap: 0x%X", size, heap);

    if (size < heap->available_size) {

        /* search on free list */
        free_size = 0;

        /* get the first free memory block */
        header_ptr = heap->free_list->next_free;
        while (header_ptr != heap->free_list && free_size < size) {

            /* get current freed memory block size */
            free_size = __MEMITEM_SIZE(header_ptr);
            if (free_size < size) {
                /* move to next free memory block */
                header_ptr = header_ptr->next_free;
            }
        }

        /* determine if the memory is available. */
        if (free_size >= size) {
            /* a block that satisfies the request has been found. */

            /* determine if the block needs to be split. */
            if (free_size >= (size + __MEMHEAP_SIZE + __MEMHEAP_MINALLOC)){

                struct am_memheap_item *new_ptr;

                /* split the block. */
                new_ptr = (struct am_memheap_item *)
                          (((uint8_t *)header_ptr) + size + __MEMHEAP_SIZE);

                log_d("split: block[0x%08X] nextm[0x%08X] prevm[0x%08X] to new[0x%08X]\n",
                          header_ptr,
                          header_ptr->next,
                          header_ptr->prev,
                          new_ptr);

                /* mark the new block as a memory block and freed. */
                new_ptr->magic = __MEMHEAP_MAGIC;

                /* put the pool pointer into the new block. */
                new_ptr->pool_ptr = heap;

                /* break down the block list */
                new_ptr->prev          = header_ptr;
                new_ptr->next          = header_ptr->next;
                header_ptr->next->prev = new_ptr;
                header_ptr->next       = new_ptr;

                /* remove header ptr from free list */
                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;

                /* insert new_ptr to free list */
                new_ptr->next_free = heap->free_list->next_free;
                new_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = new_ptr;
                heap->free_list->next_free            = new_ptr;
                log_d("new ptr: next_free 0x%08X, prev_free 0x%08X\n",
                         new_ptr->next_free,
                         new_ptr->prev_free);

                /* decrement the available byte count.  */
                heap->available_size = heap->available_size -
                                       size -
                                       __MEMHEAP_SIZE;
                if (heap->pool_size - heap->available_size > heap->max_used_size) {
                    heap->max_used_size = heap->pool_size - heap->available_size;
                }

            } else {

                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - free_size;
                if (heap->pool_size - heap->available_size > heap->max_used_size) {
                    heap->max_used_size = heap->pool_size - heap->available_size;
                }
                /* remove header_ptr from free list */
                log_d("one block: block[0x%08X], next_free 0x%08X, prev_free 0x%08X\n",
                         header_ptr,
                         header_ptr->next_free,
                         header_ptr->prev_free);

                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;
            }

            /* Mark the allocated block as not available. */
            header_ptr->magic    |= __MEMHEAP_USED;

            /* Return a memory address to the caller.  */
            log_d("alloc mem: memory[0x%08X], heap[0x%08X], size: %d\n",
                     (void *)((uint8_t *)header_ptr + __MEMHEAP_SIZE),
                      header_ptr,
                      size);

            return (void *)((uint8_t *)header_ptr + __MEMHEAP_SIZE);
        }
    }

    log_d(("allocate memory: failed\n"));

    /* Return the completion status.  */
    return NULL;
}

size_t am_memheap_memsize(struct am_memheap *heap, void *ptr)
{
    struct am_memheap_item *header_ptr;

    (void)heap;
    /* get memory block header and get the size of memory block */
    header_ptr = (struct am_memheap_item *)
                 ((uint8_t *)ptr - __MEMHEAP_SIZE);
    return __MEMITEM_SIZE(header_ptr);
}

/******************************************************************************/
void *am_memheap_realloc(struct am_memheap *heap, void *ptr, size_t newsize)
{
    size_t oldsize;
    struct am_memheap_item *header_ptr;
    struct am_memheap_item *new_ptr;

    if (newsize == 0) {
        am_memheap_free(ptr);

        return NULL;
    }
    /* align allocated size */
    newsize = FM_ALIGN(newsize, __MEM_ALIGN_SIZE);
    if (newsize < __MEMHEAP_MINALLOC) {
        newsize = __MEMHEAP_MINALLOC;
    }

    if (ptr == NULL) {
        return am_memheap_alloc(heap, newsize);
    }

    /* get memory block header and get the size of memory block */
    header_ptr = (struct am_memheap_item *)
                 ((uint8_t *)ptr - __MEMHEAP_SIZE);
    oldsize = __MEMITEM_SIZE(header_ptr);

    /* re-allocate memory */
    if (newsize > oldsize) {
        void* new_ptr;
        struct am_memheap_item *next_ptr;

        next_ptr = header_ptr->next;

        /* header_ptr should not be the tail */
        FM_ASSERT(next_ptr > header_ptr);

        /* check whether the following free space is enough to expand */
        if (!__MEMHEAP_IS_USED(next_ptr)) {
            int32_t nextsize;

            nextsize = __MEMITEM_SIZE(next_ptr);
            FM_ASSERT(next_ptr > 0);

            /* Here is the ASCII art of the situation that we can make use of
             * the next free node without alloc/memcpy, |*| is the control
             * block:
             *
             *      oldsize           free node
             * |*|-----------|*|----------------------|*|
             *         newsize          >= minialloc
             * |*|----------------|*|-----------------|*|
             */
            if (nextsize + oldsize > newsize + __MEMHEAP_MINALLOC) {

                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - (newsize - oldsize);
                if (heap->pool_size - heap->available_size > heap->max_used_size) {
                    heap->max_used_size = heap->pool_size - heap->available_size;
                }

                /* remove next_ptr from free list */
                log_d("remove block: block[0x%08X], next_free 0x%08X, prev_free 0x%08X",
                         next_ptr,
                         next_ptr->next_free,
                         next_ptr->prev_free);

                next_ptr->next_free->prev_free = next_ptr->prev_free;
                next_ptr->prev_free->next_free = next_ptr->next_free;
                next_ptr->next->prev = next_ptr->prev;
                next_ptr->prev->next = next_ptr->next;

                /* build a new one on the right place */
                next_ptr = (struct am_memheap_item*)((char*)ptr + newsize);

                log_d("new free block: block[0x%08X] nextm[0x%08X] prevm[0x%08X]",
                         next_ptr,
                         next_ptr->next,
                         next_ptr->prev);

                /* mark the new block as a memory block and freed. */
                next_ptr->magic = __MEMHEAP_MAGIC;

                /* put the pool pointer into the new block. */
                next_ptr->pool_ptr = heap;

                next_ptr->prev          = header_ptr;
                next_ptr->next          = header_ptr->next;
                header_ptr->next->prev = next_ptr;
                header_ptr->next       = next_ptr;

                /* insert next_ptr to free list */
                next_ptr->next_free = heap->free_list->next_free;
                next_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = next_ptr;
                heap->free_list->next_free            = next_ptr;
                log_d("new ptr: next_free 0x%08X, prev_free 0x%08X",
                         next_ptr->next_free,
                         next_ptr->prev_free);

                return ptr;
            }
        }

        /* re-allocate a memory block */
        new_ptr = (void*)am_memheap_alloc(heap, newsize);
        if (new_ptr != NULL) {
            memcpy(new_ptr, ptr, oldsize < newsize ? oldsize : newsize);
            am_memheap_free(ptr);
        }

        return new_ptr;
    }

    /* don't split when there is less than one node space left */
    if (newsize + __MEMHEAP_SIZE + __MEMHEAP_MINALLOC >= oldsize) {
        return ptr;
    }

    /* split the block. */
    new_ptr = (struct am_memheap_item *)
              (((uint8_t *)header_ptr) + newsize + __MEMHEAP_SIZE);

    log_d("split: block[0x%08X] nextm[0x%08X] prevm[0x%08X] to new[0x%08X]\n",
             header_ptr,
             header_ptr->next,
             header_ptr->prev,
             new_ptr);

    /* mark the new block as a memory block and freed. */
    new_ptr->magic = __MEMHEAP_MAGIC;
    /* put the pool pointer into the new block. */
    new_ptr->pool_ptr = heap;

    /* break down the block list */
    new_ptr->prev          = header_ptr;
    new_ptr->next          = header_ptr->next;
    header_ptr->next->prev = new_ptr;
    header_ptr->next       = new_ptr;

    /* determine if the block can be merged with the next neighbor. */
    if (!__MEMHEAP_IS_USED(new_ptr->next)) {
        struct am_memheap_item *free_ptr;

        /* merge block with next neighbor. */
        free_ptr = new_ptr->next;
        heap->available_size = heap->available_size - __MEMITEM_SIZE(free_ptr);

        log_d("merge: right node 0x%08X, next_free 0x%08X, prev_free 0x%08X\n",
                 header_ptr, header_ptr->next_free, header_ptr->prev_free);

        free_ptr->next->prev = new_ptr;
        new_ptr->next   = free_ptr->next;

        /* remove free ptr from free list */
        free_ptr->next_free->prev_free = free_ptr->prev_free;
        free_ptr->prev_free->next_free = free_ptr->next_free;
    }

    /* insert the split block to free list */
    new_ptr->next_free = heap->free_list->next_free;
    new_ptr->prev_free = heap->free_list;
    heap->free_list->next_free->prev_free = new_ptr;
    heap->free_list->next_free            = new_ptr;
    log_d("new free ptr: next_free 0x%08X, prev_free 0x%08X\n",
             new_ptr->next_free,
             new_ptr->prev_free);

    /* increment the available byte count.  */
    heap->available_size = heap->available_size + __MEMITEM_SIZE(new_ptr);

    /* return the old memory block */
    return ptr;
}

/******************************************************************************/
void am_memheap_free(void *ptr)
{
    struct am_memheap *heap;
    struct am_memheap_item *header_ptr, *new_ptr;
    uint32_t insert_header;

	/* NULL check */
	if (ptr == NULL) return;

    /* set initial status as OK */
    insert_header = 1;
    new_ptr       = NULL;
    header_ptr    = (struct am_memheap_item *)
                    ((uint8_t *)ptr - __MEMHEAP_SIZE);

    log_d("free memory: memory[0x%08X], block[0x%08X]\n",
             ptr, header_ptr);

    /* check magic */
    FM_ASSERT((header_ptr->magic & __MEMHEAP_MASK) == __MEMHEAP_MAGIC);

    /* get pool ptr */
    heap = header_ptr->pool_ptr;

    /* Mark the memory as available. */
    header_ptr->magic &= ~__MEMHEAP_USED;
    /* Adjust the available number of bytes. */
    heap->available_size = heap->available_size + __MEMITEM_SIZE(header_ptr);

    /* Determine if the block can be merged with the previous neighbor. */
    if (!__MEMHEAP_IS_USED(header_ptr->prev)) {
        log_d("merge: left node 0x%08X\n", header_ptr->prev);

        /* adjust the available number of bytes. */
        heap->available_size = heap->available_size + __MEMHEAP_SIZE;

        /* yes, merge block with previous neighbor. */
        (header_ptr->prev)->next = header_ptr->next;
        (header_ptr->next)->prev = header_ptr->prev;

        /* move header pointer to previous. */
        header_ptr = header_ptr->prev;
        /* don't insert header to free list */
        insert_header = 0;
    }

    /* determine if the block can be merged with the next neighbor. */
    if (!__MEMHEAP_IS_USED(header_ptr->next)) {
        /* adjust the available number of bytes. */
        heap->available_size = heap->available_size + __MEMHEAP_SIZE;

        /* merge block with next neighbor. */
        new_ptr = header_ptr->next;

        log_d("merge: right node 0x%08X, next_free 0x%08X, prev_free 0x%08X\n",
                 new_ptr, new_ptr->next_free, new_ptr->prev_free);

        new_ptr->next->prev = header_ptr;
        header_ptr->next    = new_ptr->next;

        /* remove new ptr from free list */
        new_ptr->next_free->prev_free = new_ptr->prev_free;
        new_ptr->prev_free->next_free = new_ptr->next_free;
    }

    if (insert_header) {
        /* no left merge, insert to free list */
        header_ptr->next_free = heap->free_list->next_free;
        header_ptr->prev_free = heap->free_list;
        heap->free_list->next_free->prev_free = header_ptr;
        heap->free_list->next_free            = header_ptr;

        log_d("insert to free list: next_free 0x%08X, prev_free 0x%08X\n",
                 header_ptr->next_free, header_ptr->prev_free);
    }
}

/* fastMCU adapt */

#define MEM_SIZE (((MEM_BUF_SIZE) >> 2) << 2)

#if defined(__CC_ARM)
__attribute__((aligned(4))) static uint8_t fm_mem_buf[MEM_SIZE];
#elif defined(__ICCARM__)
#pragma data_alignment = 4
static uint8_t fm_mem_buf[MEM_SIZE];
#elif defined(__GNUC__)
__attribute__((aligned(4))) static uint8_t fm_mem_buf[MEM_SIZE];
#endif

static struct am_memheap fm_memheap_ifno;

static void fm_memheap_init(void)
{
    am_memheap_init(&fm_memheap_ifno, "fm_memheap", fm_mem_buf, MEM_SIZE);
}

INITLV0_EXPORT(fm_memheap_init);

void *fm_malloc(uint32_t size)
{
    if (size == 0)
    {
        return NULL;
    }
    return am_memheap_alloc(&fm_memheap_ifno, size);
}

void fm_free(void *paddr)
{
    am_memheap_free(paddr);
}

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

void *fm_realloc(void *paddr, uint32_t newsize)
{
    return am_memheap_realloc(&fm_memheap_ifno, paddr, newsize);
}

uint32_t fm_get_free_size(void)
{
    return 0xFFFFFFFF;
}
