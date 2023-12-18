#include <fmcu.h>

#ifdef FM_USING_MEMHEAP

#define DBG_TAG "memheap"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

/* dynamic pool magic and mask */
#define FM_MEMHEAP_MAGIC        0x1ea01ea0
#define FM_MEMHEAP_MASK         0xFFFFFFFE
#define FM_MEMHEAP_USED         0x01
#define FM_MEMHEAP_FREED        0x00

#define FM_MEMHEAP_IS_USED(i)   ((i)->magic & FM_MEMHEAP_USED)
#define FM_MEMHEAP_MINIALLOC    FM_ALIGN(12, FM_ALIGN_SIZE)

#define FM_MEMHEAP_SIZE         FM_ALIGN(sizeof(struct fm_memheap_item), FM_ALIGN_SIZE)
#define MEMITEM_SIZE(item)      ((fm_ubase_t)item->next - (fm_ubase_t)item - FM_MEMHEAP_SIZE)
#define MEMITEM(ptr)            (struct fm_memheap_item*)((fm_uint8_t*)ptr - FM_MEMHEAP_SIZE)

static void _remove_next_ptr(volatile struct fm_memheap_item *next_ptr)
{
    /* Fix the crash problem after opening Oz optimization on ac6  */
    /* Fix IAR compiler warning  */
    next_ptr->next_free->prev_free = next_ptr->prev_free;
    next_ptr->prev_free->next_free = next_ptr->next_free;
    next_ptr->next->prev = next_ptr->prev;
    next_ptr->prev->next = next_ptr->next;
}

/**
 * @brief   This function initializes a piece of memory called memheap.
 *
 * @note    The initialized memory pool will be:
 *          +-----------------------------------+--------------------------+
 *          | whole freed memory block          | Used Memory Block Tailer |
 *          +-----------------------------------+--------------------------+
 *
 *          block_list --> whole freed memory block
 *
 *          The length of Used Memory Block Tailer is 0,
 *          which is prevents block merging across list
 *
 * @param   memheap is a pointer of the memheap object.
 *
 * @param   name is the name of the memheap.
 *
 * @param   start_addr is the start address of the memheap.
 *
 * @param   size is the size of the memheap.
 *
 * @return  FM_EOK
 */
fm_err_t fm_memheap_init(struct fm_memheap *memheap,
                         const char        *name,
                         void              *start_addr,
                         fm_size_t         size)
{
    struct fm_memheap_item *item;

    FM_ASSERT(memheap != FM_NULL);

    fm_strncpy(memheap->name, name, FM_NAME_MAX);  /* copy name */
    memheap->start_addr     = start_addr;
    memheap->pool_size      = FM_ALIGN_DOWN(size, FM_ALIGN_SIZE);
    memheap->available_size = memheap->pool_size - (2 * FM_MEMHEAP_SIZE);
    memheap->max_used_size  = memheap->pool_size - memheap->available_size;

    /* initialize the free list header */
    item            = &(memheap->free_header);
    item->magic     = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_FREED);
    item->pool_ptr  = memheap;
    item->next      = FM_NULL;
    item->prev      = FM_NULL;
    item->next_free = item;
    item->prev_free = item;

    /* set the free list to free list header */
    memheap->free_list = item;

    /* initialize the first big memory block */
    item            = (struct fm_memheap_item *)start_addr;
    item->magic     = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_FREED);
    item->pool_ptr  = memheap;
    item->next      = FM_NULL;
    item->prev      = FM_NULL;
    item->next_free = item;
    item->prev_free = item;

    item->next = (struct fm_memheap_item *)
                 ((fm_uint8_t *)item + memheap->available_size + FM_MEMHEAP_SIZE);
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
    item->magic     = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_USED);
    item->pool_ptr  = memheap;
    item->next      = (struct fm_memheap_item *)start_addr;
    item->prev      = (struct fm_memheap_item *)start_addr;
    /* not in free list */
    item->next_free = item->prev_free = FM_NULL;

    LOG_D("memory heap: start addr 0x%08x, size %d, free list header 0x%08x",
          start_addr, size, &(memheap->free_header));

    return FM_EOK;
}

/**
 * @brief  Allocate a block of memory with a minimum of 'size' bytes on memheap.
 *
 * @param   heap is a pointer for memheap object.
 *
 * @param   size is the minimum size of the requested block in bytes.
 *
 * @return  the pointer to allocated memory or NULL if no free memory was found.
 */
void *fm_memheap_alloc(struct fm_memheap *heap, fm_size_t size)
{
    fm_size_t free_size;
    struct fm_memheap_item *header_ptr;

    FM_ASSERT(heap != FM_NULL);

    /* align allocated size */
    size = FM_ALIGN(size, FM_ALIGN_SIZE);
    if (size < FM_MEMHEAP_MINIALLOC)
        size = FM_MEMHEAP_MINIALLOC;

    LOG_D("allocate %d on heap:%8.*s",
          size, FM_NAME_MAX, heap->name);

    if (size < heap->available_size)
    {
        /* search on free list */
        free_size = 0;

        /* get the first free memory block */
        header_ptr = heap->free_list->next_free;
        while (header_ptr != heap->free_list && free_size < size)
        {
            /* get current freed memory block size */
            free_size = MEMITEM_SIZE(header_ptr);
            if (free_size < size)
            {
                /* move to next free memory block */
                header_ptr = header_ptr->next_free;
            }
        }

        /* determine if the memory is available. */
        if (free_size >= size)
        {
            /* a block that satisfies the request has been found. */

            /* determine if the block needs to be split. */
            if (free_size >= (size + FM_MEMHEAP_SIZE + FM_MEMHEAP_MINIALLOC))
            {
                struct fm_memheap_item *new_ptr;

                /* split the block. */
                new_ptr = (struct fm_memheap_item *)
                          (((fm_uint8_t *)header_ptr) + size + FM_MEMHEAP_SIZE);

                LOG_D("split: block[0x%08x] nextm[0x%08x] prevm[0x%08x] to new[0x%08x]",
                      header_ptr,
                      header_ptr->next,
                      header_ptr->prev,
                      new_ptr);

                /* mark the new block as a memory block and freed. */
                new_ptr->magic = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_FREED);

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
                header_ptr->next_free = FM_NULL;
                header_ptr->prev_free = FM_NULL;

                /* insert new_ptr to free list */
                new_ptr->next_free = heap->free_list->next_free;
                new_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = new_ptr;
                heap->free_list->next_free            = new_ptr;
                LOG_D("new ptr: next_free 0x%08x, prev_free 0x%08x",
                      new_ptr->next_free,
                      new_ptr->prev_free);

                /* decrement the available byte count.  */
                heap->available_size = heap->available_size -
                                       size -
                                       FM_MEMHEAP_SIZE;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;
            }
            else
            {
                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - free_size;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;

                /* remove header_ptr from free list */
                LOG_D("one block: block[0x%08x], next_free 0x%08x, prev_free 0x%08x",
                      header_ptr,
                      header_ptr->next_free,
                      header_ptr->prev_free);

                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = FM_NULL;
                header_ptr->prev_free = FM_NULL;
            }

            /* Mark the allocated block as not available. */
            header_ptr->magic = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_USED);

            /* Return a memory address to the caller.  */
            LOG_D("alloc mem: memory[0x%08x], heap[0x%08x], size: %d",
                  (void *)((fm_uint8_t *)header_ptr + FM_MEMHEAP_SIZE),
                  header_ptr,
                  size);

            return (void *)((fm_uint8_t *)header_ptr + FM_MEMHEAP_SIZE);
        }
    }

    LOG_D("allocate memory: failed");

    /* Return the completion status.  */
    return FM_NULL;
}

/**
 * @brief This function will change the size of previously allocated memory block.
 *
 * @param heap is a pointer to the memheap object, which will reallocate
 *             memory from the block
 *
 * @param ptr is a pointer to start address of memory.
 *
 * @param newsize is the required new size.
 *
 * @return the changed memory block address.
 */
void *fm_memheap_realloc(struct fm_memheap *heap, void *ptr, fm_size_t newsize)
{
    fm_size_t oldsize;
    struct fm_memheap_item *header_ptr;
    struct fm_memheap_item *new_ptr;

    FM_ASSERT(heap);

    if (newsize == 0)
    {
        fm_memheap_free(ptr);

        return FM_NULL;
    }
    /* align allocated size */
    newsize = FM_ALIGN(newsize, FM_ALIGN_SIZE);
    if (newsize < FM_MEMHEAP_MINIALLOC)
        newsize = FM_MEMHEAP_MINIALLOC;

    if (ptr == FM_NULL)
    {
        return fm_memheap_alloc(heap, newsize);
    }

    /* get memory block header and get the size of memory block */
    header_ptr = (struct fm_memheap_item *)
                 ((fm_uint8_t *)ptr - FM_MEMHEAP_SIZE);
    oldsize = MEMITEM_SIZE(header_ptr);
    /* re-allocate memory */
    if (newsize > oldsize)
    {
        void *new_ptr;
        volatile struct fm_memheap_item *next_ptr;

        next_ptr = header_ptr->next;

        /* header_ptr should not be the tail */
        FM_ASSERT(next_ptr > header_ptr);

        /* check whether the following free space is enough to expand */
        if (!FM_MEMHEAP_IS_USED(next_ptr))
        {
            fm_int32_t nextsize;

            nextsize = MEMITEM_SIZE(next_ptr);
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
            if (nextsize + oldsize > newsize + FM_MEMHEAP_MINIALLOC)
            {
                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - (newsize - oldsize);
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;

                /* remove next_ptr from free list */
                LOG_D("remove block: block[0x%08x], next_free 0x%08x, prev_free 0x%08x",
                      next_ptr,
                      next_ptr->next_free,
                      next_ptr->prev_free);

                _remove_next_ptr(next_ptr);

                /* build a new one on the right place */
                next_ptr = (struct fm_memheap_item *)((char *)ptr + newsize);

                LOG_D("new free block: block[0x%08x] nextm[0x%08x] prevm[0x%08x]",
                      next_ptr,
                      next_ptr->next,
                      next_ptr->prev);

                /* mark the new block as a memory block and freed. */
                next_ptr->magic = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_FREED);

                /* put the pool pointer into the new block. */
                next_ptr->pool_ptr = heap;

                next_ptr->prev          = header_ptr;
                next_ptr->next          = header_ptr->next;
                header_ptr->next->prev = (struct fm_memheap_item *)next_ptr;
                header_ptr->next       = (struct fm_memheap_item *)next_ptr;

                /* insert next_ptr to free list */
                next_ptr->next_free = heap->free_list->next_free;
                next_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = (struct fm_memheap_item *)next_ptr;
                heap->free_list->next_free            = (struct fm_memheap_item *)next_ptr;
                LOG_D("new ptr: next_free 0x%08x, prev_free 0x%08x",
                      next_ptr->next_free,
                      next_ptr->prev_free);

                return ptr;
            }
        }

        /* re-allocate a memory block */
        new_ptr = (void *)fm_memheap_alloc(heap, newsize);
        if (new_ptr != FM_NULL)
        {
            fm_memcpy(new_ptr, ptr, oldsize < newsize ? oldsize : newsize);
            fm_memheap_free(ptr);
        }

        return new_ptr;
    }

    /* don't split when there is less than one node space left */
    if (newsize + FM_MEMHEAP_SIZE + FM_MEMHEAP_MINIALLOC >= oldsize)
        return ptr;

    /* split the block. */
    new_ptr = (struct fm_memheap_item *)
              (((fm_uint8_t *)header_ptr) + newsize + FM_MEMHEAP_SIZE);

    LOG_D("split: block[0x%08x] nextm[0x%08x] prevm[0x%08x] to new[0x%08x]",
          header_ptr,
          header_ptr->next,
          header_ptr->prev,
          new_ptr);

    /* mark the new block as a memory block and freed. */
    new_ptr->magic = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_FREED);
    /* put the pool pointer into the new block. */
    new_ptr->pool_ptr = heap;

    /* break down the block list */
    new_ptr->prev          = header_ptr;
    new_ptr->next          = header_ptr->next;
    header_ptr->next->prev = new_ptr;
    header_ptr->next       = new_ptr;

    /* determine if the block can be merged with the next neighbor. */
    if (!FM_MEMHEAP_IS_USED(new_ptr->next))
    {
        struct fm_memheap_item *free_ptr;

        /* merge block with next neighbor. */
        free_ptr = new_ptr->next;
        heap->available_size = heap->available_size - MEMITEM_SIZE(free_ptr);

        LOG_D("merge: right node 0x%08x, next_free 0x%08x, prev_free 0x%08x",
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
    LOG_D("new free ptr: next_free 0x%08x, prev_free 0x%08x",
          new_ptr->next_free,
          new_ptr->prev_free);

    /* increment the available byte count.  */
    heap->available_size = heap->available_size + MEMITEM_SIZE(new_ptr);

    /* return the old memory block */
    return ptr;
}

/**
 * @brief This function will release the allocated memory block by
 *        fm_malloc. The released memory block is taken back to system heap.
 *
 * @param ptr the address of memory which will be released.
 */
void fm_memheap_free(void *ptr)
{
    struct fm_memheap *heap;
    struct fm_memheap_item *header_ptr, *new_ptr;
    fm_bool_t insert_header;

    /* NULL check */
    if (ptr == FM_NULL) return;

    /* set initial status as OK */
    insert_header = FM_TRUE;
    new_ptr       = FM_NULL;
    header_ptr    = (struct fm_memheap_item *)
                    ((fm_uint8_t *)ptr - FM_MEMHEAP_SIZE);

    LOG_D("free memory: memory[0x%08x], block[0x%08x]",
          ptr, header_ptr);

    /* check magic */
    if (header_ptr->magic != (FM_MEMHEAP_MAGIC | FM_MEMHEAP_USED) ||
       (header_ptr->next->magic & FM_MEMHEAP_MASK) != FM_MEMHEAP_MAGIC)
    {
        LOG_D("bad magic:0x%08x @ memheap",
              header_ptr->magic);
        FM_ASSERT(header_ptr->magic == (FM_MEMHEAP_MAGIC | FM_MEMHEAP_USED));
        /* check whether this block of memory has been over-written. */
        FM_ASSERT((header_ptr->next->magic & FM_MEMHEAP_MASK) == FM_MEMHEAP_MAGIC);
    }

    /* get pool ptr */
    heap = header_ptr->pool_ptr;

    FM_ASSERT(heap);

    /* Mark the memory as available. */
    header_ptr->magic = (FM_MEMHEAP_MAGIC | FM_MEMHEAP_FREED);
    /* Adjust the available number of bytes. */
    heap->available_size += MEMITEM_SIZE(header_ptr);

    /* Determine if the block can be merged with the previous neighbor. */
    if (!FM_MEMHEAP_IS_USED(header_ptr->prev))
    {
        LOG_D("merge: left node 0x%08x",
              header_ptr->prev);

        /* adjust the available number of bytes. */
        heap->available_size += FM_MEMHEAP_SIZE;

        /* yes, merge block with previous neighbor. */
        (header_ptr->prev)->next = header_ptr->next;
        (header_ptr->next)->prev = header_ptr->prev;

        /* move header pointer to previous. */
        header_ptr = header_ptr->prev;
        /* don't insert header to free list */
        insert_header = FM_FALSE;
    }

    /* determine if the block can be merged with the next neighbor. */
    if (!FM_MEMHEAP_IS_USED(header_ptr->next))
    {
        /* adjust the available number of bytes. */
        heap->available_size += FM_MEMHEAP_SIZE;

        /* merge block with next neighbor. */
        new_ptr = header_ptr->next;

        LOG_D("merge: right node 0x%08x, next_free 0x%08x, prev_free 0x%08x",
              new_ptr, new_ptr->next_free, new_ptr->prev_free);

        new_ptr->next->prev = header_ptr;
        header_ptr->next    = new_ptr->next;

        /* remove new ptr from free list */
        new_ptr->next_free->prev_free = new_ptr->prev_free;
        new_ptr->prev_free->next_free = new_ptr->next_free;
    }

    if (insert_header)
    {
        struct fm_memheap_item *n = heap->free_list->next_free;;
#if defined(FM_MEMHEAP_BEST_MODE)
        fm_size_t blk_size = MEMITEM_SIZE(header_ptr);
        for (;n != heap->free_list; n = n->next_free)
        {
            fm_size_t m = MEMITEM_SIZE(n);
            if (blk_size <= m)
            {
                break;
            }
        }
#endif
        /* no left merge, insert to free list */
        header_ptr->next_free = n;
        header_ptr->prev_free = n->prev_free;
        n->prev_free->next_free = header_ptr;
        n->prev_free = header_ptr;

        LOG_D("insert to free list: next_free 0x%08x, prev_free 0x%08x",
              header_ptr->next_free, header_ptr->prev_free);
    }
}

/**
* @brief This function will caculate the total memory, the used memory, and
*        the max used memory.
*
* @param heap is a pointer to the memheap object, which will reallocate
*             memory from the block
*
* @param total is a pointer to get the total size of the memory.
*
* @param used is a pointer to get the size of memory used.
*
* @param max_used is a pointer to get the maximum memory used.
*/
void fm_memheap_info(struct fm_memheap *heap,
                     fm_size_t *total,
                     fm_size_t *used,
                     fm_size_t *max_used)
{
    if (total != FM_NULL)
        *total = heap->pool_size;

    if (used  != FM_NULL)
        *used = heap->pool_size - heap->available_size;

    if (max_used != FM_NULL)
        *max_used = heap->max_used_size;
}

#ifdef FM_USING_MEMHEAP_AS_HEAP
/*
 * fm_malloc port function
*/
void *_memheap_alloc(struct fm_memheap *heap, fm_size_t size)
{
    void *ptr;

    /* try to allocate in system heap */
    ptr = fm_memheap_alloc(heap, size);

    return ptr;
}

/*
 * rt_free port function
*/
void _memheap_free(void *rmem)
{
    fm_memheap_free(rmem);
}

/*
 * rt_realloc port function
*/
void *_memheap_realloc(struct fm_memheap *heap, void *rmem, fm_size_t newsize)
{
    void *new_ptr;
    struct fm_memheap_item *header_ptr;

    if (rmem == FM_NULL)
        return _memheap_alloc(heap, newsize);

    if (newsize == 0)
    {
        _memheap_free(rmem);
        return FM_NULL;
    }

    /* get old memory item */
    header_ptr = (struct fm_memheap_item *)
                 ((fm_uint8_t *)rmem - FM_MEMHEAP_SIZE);

    new_ptr = fm_memheap_realloc(header_ptr->pool_ptr, rmem, newsize);
    if (new_ptr == FM_NULL && newsize != 0)
    {
        /* allocate memory block from other memheap */
        new_ptr = _memheap_alloc(heap, newsize);
        if (new_ptr != FM_NULL && rmem != FM_NULL)
        {
            fm_size_t oldsize;

            /* get the size of old memory block */
            oldsize = MEMITEM_SIZE(header_ptr);
            if (newsize > oldsize)
                fm_memcpy(new_ptr, rmem, oldsize);
            else
                fm_memcpy(new_ptr, rmem, newsize);

            _memheap_free(rmem);
        }
    }

    return new_ptr;
}
#endif

#endif /* FM_USING_MEMHEAP */
