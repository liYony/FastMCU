#include <fmcu.h>

#if defined(FM_USING_DEVICE) && defined(FM_USING_CONSOLE)
static fm_device_t _console_device = FM_NULL;
#endif

/**
 * @brief  This function will set the content of memory to specified value.
 *
 * @param  s is the address of source memory, point to the memory block to be filled.
 *
 * @param  c is the value to be set. The value is passed in int form, but the function
 *         uses the unsigned character form of the value when filling the memory block.
 *
 * @param  count number of bytes to be set.
 *
 * @return The address of source memory.
 */
fm_weak void *fm_memset(void *s, int c, fm_ubase_t count)
{
#ifdef FM_KSERVICE_USING_TINY_SIZE
    char *xs = (char *)s;

    while (count--)
        *xs++ = c;

    return s;
#else
#define LBLOCKSIZE      (sizeof(fm_ubase_t))
#define UNALIGNED(X)    ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

    unsigned int i = 0;
    char *m = (char *)s;
    unsigned long buffer = 0;
    unsigned long *aligned_addr = FM_NULL;
    unsigned char d = (unsigned int)c & (unsigned char)(-1);  /* To avoid sign extension, copy C to an
    unsigned variable. (unsigned)((char)(-1))=0xFF for 8bit and =0xFFFF for 16bit: word independent */

    FM_ASSERT(LBLOCKSIZE == 2 || LBLOCKSIZE == 4 || LBLOCKSIZE == 8);

    if (!TOO_SMALL(count) && !UNALIGNED(s))
    {
        /* If we get this far, we know that count is large and s is word-aligned. */
        aligned_addr = (unsigned long *)s;

        /* Store d into each char sized location in buffer so that
         * we can set large blocks quickly.
         */
        for (i = 0; i < LBLOCKSIZE; i++)
        {
            *(((unsigned char *)&buffer)+i) = d;
        }

        while (count >= LBLOCKSIZE * 4)
        {
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            count -= 4 * LBLOCKSIZE;
        }

        while (count >= LBLOCKSIZE)
        {
            *aligned_addr++ = buffer;
            count -= LBLOCKSIZE;
        }

        /* Pick up the remainder with a bytewise loop. */
        m = (char *)aligned_addr;
    }

    while (count--)
    {
        *m++ = (char)d;
    }

    return s;

#undef LBLOCKSIZE
#undef UNALIGNED
#undef TOO_SMALL
#endif /* FM_KSERVICE_USING_TINY_SIZE */
}

/**
 * @brief  This function will copy memory content from source address to destination address.
 *
 * @param  dst is the address of destination memory, points to the copied content.
 *
 * @param  src  is the address of source memory, pointing to the data source to be copied.
 *
 * @param  count is the copied length.
 *
 * @return The address of destination memory
 */
fm_weak void *fm_memcpy(void *dst, const void *src, fm_ubase_t count)
{
#ifdef FM_KSERVICE_USING_TINY_SIZE
    char *tmp = (char *)dst, *s = (char *)src;
    fm_ubase_t len = 0;

    if (tmp <= s || tmp > (s + count))
    {
        while (count--)
            *tmp ++ = *s ++;
    }
    else
    {
        for (len = count; len > 0; len --)
            tmp[len - 1] = s[len - 1];
    }

    return dst;
#else

#define UNALIGNED(X, Y) \
    (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#define BIGBLOCKSIZE    (sizeof (long) << 2)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

    char *dst_ptr = (char *)dst;
    char *src_ptr = (char *)src;
    long *aligned_dst = FM_NULL;
    long *aligned_src = FM_NULL;
    fm_ubase_t len = count;

    /* If the size is small, or either SRC or DST is unaligned,
    then punt into the byte copy loop.  This should be rare. */
    if (!TOO_SMALL(len) && !UNALIGNED(src_ptr, dst_ptr))
    {
        aligned_dst = (long *)dst_ptr;
        aligned_src = (long *)src_ptr;

        /* Copy 4X long words at a time if possible. */
        while (len >= BIGBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible. */
        while (len >= LITTLEBLOCKSIZE)
        {
            *aligned_dst++ = *aligned_src++;
            len -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier. */
        dst_ptr = (char *)aligned_dst;
        src_ptr = (char *)aligned_src;
    }

    while (len--)
        *dst_ptr++ = *src_ptr++;

    return dst;
#undef UNALIGNED
#undef BIGBLOCKSIZE
#undef LITTLEBLOCKSIZE
#undef TOO_SMALL
#endif /* FM_KSERVICE_USING_TINY_SIZE */
}

/**
 * @brief  This function will return the length of a string, which terminate will
 * null character.
 *
 * @param  s is the string
 *
 * @return The length of string.
 */
fm_size_t fm_strlen(const char *s)
{
    const char *sc = FM_NULL;

    for (sc = s; *sc != '\0'; ++sc) /* nothing */
        ;

    return sc - s;
}

/**
 * This function will copy string no more than n bytes.
 *
 * @param  dst points to the address used to store the copied content.
 *
 * @param  src is the string to be copied.
 *
 * @param  n is the maximum copied length.
 *
 * @return The address where the copied content is stored.
 */
char *fm_strncpy(char *dst, const char *src, fm_size_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                {
                    *d++ = 0;
                }

                break;
            }
        } while (--n != 0);
    }

    return (dst);
}

/**
 * This function will copy string.
 *
 * @param  dst points to the address used to store the copied content.
 *
 * @param  src is the string to be copied.
 *
 * @return The address where the copied content is stored.
 */
char *fm_strcpy(char *dst, const char *src)
{
    char *dest = dst;

    while (*src != '\0')
    {
        *dst = *src;
        dst++;
        src++;
    }

    *dst = '\0';
    return dest;
}

/**
 * This function will compare two strings with specified maximum length.
 *
 * @param  cs is the string to be compared.
 *
 * @param  ct is the string to be compared.
 *
 * @param  count is the maximum compare length.
 *
 * @return Compare the results:
 *         If the result < 0, cs is smaller than ct.
 *         If the result > 0, cs is greater than ct.
 *         If the result = 0, cs is equal to ct.
 */
fm_int32_t fm_strncmp(const char *cs, const char *ct, fm_size_t count)
{
    signed char __res = 0;

    while (count)
    {
        if ((__res = *cs - *ct++) != 0 || !*cs++)
        {
            break;
        }

        count --;
    }

    return __res;
}

/**
 * @brief This function will show the version of FastMCU
 */
void fm_show_version(void)
{
    fm_kprintf("\n \\ | /\r\n");
    fm_kprintf("- FM -     FastMCU Operating System\r\n");
    fm_kprintf(" / | \\     %d.%d.%d build %s %s\r\n",
                (fm_int32_t)FM_VERSION_MAJOR, (fm_int32_t)FM_VERSION_MINOR, 
                (fm_int32_t)FM_VERSION_PATCH, __DATE__, __TIME__);
    fm_kprintf(" 2023 - 2023 Copyright by liYony\r\n");
}

#ifdef FM_USING_CONSOLE

#ifdef FM_USING_DEVICE
/**
 * @brief  This function returns the device using in console.
 *
 * @return Returns the console device pointer or FM_NULL.
 */
fm_device_t fm_console_get_device(void)
{
    return _console_device;
}

/**
 * @brief  This function will set a device as console device.
 * After set a device to console, all output of fm_kprintf will be
 * redirected to this new device.
 *
 * @param  name is the name of new console device.
 *
 * @return the old console device handler on successful, or FM_NULL on failure.
 */
fm_device_t fm_console_set_device(const char *name)
{
    fm_device_t new_device, old_device;

    /* save old device */
    old_device = _console_device;

    /* find new console device */
    new_device = fm_device_find(name);

    /* check whether it's a same device */
    if (new_device == old_device) return FM_NULL;

    if (new_device != FM_NULL)
    {
        if (_console_device != FM_NULL)
        {
            /* close old console device */
            fm_device_close(_console_device);
        }

        /* set new console device */
        fm_device_open(new_device, FM_DEVICE_OFLAG_RDWR | FM_DEVICE_FLAG_STREAM);
        _console_device = new_device;
    }

    return old_device;
}
#endif /* FM_USING_DEVICE */

/**
 * @brief This function will put string to the console.
 *
 * @param str is the string output to the console.
 */
void fm_kputs(const char *str)
{
    if (!str)
    {
        return;
    }

#ifdef FM_USING_DEVICE
    if (_console_device == FM_NULL)
    {
        fm_hw_console_output(str);
    }
    else
    {
        fm_device_write(_console_device, 0, str, fm_strlen(str));
    }
#else
    fm_hw_console_output(str);
#endif /* FM_USING_DEVICE */
}

/**
 * @brief This function will print a formatted string on system console.
 *
 * @param fmt is the format parameters.
 *
 * @return The number of characters actually written to buffer.
 */
fm_weak int fm_kprintf(const char *fmt, ...)
{
    va_list args;
    fm_size_t length = 0;
    static char fm_log_buf[FM_CONSOLEBUF_SIZE];

    va_start(args, fmt);
    /* the return value of vsnprintf is the number of bytes that would be
     * written to buffer had if the size of the buffer been sufficiently
     * large excluding the terminating null byte. If the output string
     * would be larger than the fm_log_buf, we have to adjust the output
     * length. */
    length = vsnprintf(fm_log_buf, sizeof(fm_log_buf) - 1, fmt, args);
    if (length > FM_CONSOLEBUF_SIZE - 1)
    {
        length = FM_CONSOLEBUF_SIZE - 1;
    }

#ifdef FM_USING_DEVICE
    if (_console_device == FM_NULL)
    {
        fm_hw_console_output(fm_log_buf);
    }
    else
    {
        fm_device_write(_console_device, 0, fm_log_buf, length);
    }
#else
    fm_hw_console_output(fm_log_buf);
#endif /* FM_USING_DEVICE */
    va_end(args);

    return length;
}
#endif /* FM_USING_CONSOLE */

#if defined(FM_USING_HEAP)

#if defined(FM_USING_MEMHEAP_AS_HEAP)
static struct fm_memheap system_heap;
void *_memheap_alloc(struct fm_memheap *heap, fm_size_t size);
void _memheap_free(void *rmem);
void *_memheap_realloc(struct fm_memheap *heap, void *rmem, fm_size_t newsize);
#define _MEM_INIT(_name, _start, _size) \
    fm_memheap_init(&system_heap, _name, _start, _size)
#define _MEM_MALLOC(_size)  \
    _memheap_alloc(&system_heap, _size)
#define _MEM_REALLOC(_ptr, _newsize)    \
    _memheap_realloc(&system_heap, _ptr, _newsize)
#define _MEM_FREE(_ptr)   \
    _memheap_free(_ptr)
#define _MEM_INFO(_total, _used, _max)   \
    fm_memheap_info(&system_heap, _total, _used, _max)
#else
#define _MEM_INIT(...)
#define _MEM_MALLOC(...)     FM_NULL
#define _MEM_REALLOC(...)    FM_NULL
#define _MEM_FREE(...)
#define _MEM_INFO(...)
#endif

/**
 * @brief This function will init system heap.
 *
 * @param begin_addr the beginning address of system page.
 *
 * @param end_addr the end address of system page.
 */
fm_weak void fm_system_heap_init(void *begin_addr, void *end_addr)
{
    fm_ubase_t begin_align = FM_ALIGN((fm_ubase_t)begin_addr, FM_ALIGN_SIZE);
    fm_ubase_t end_align   = FM_ALIGN_DOWN((fm_ubase_t)end_addr, FM_ALIGN_SIZE);

    FM_ASSERT(end_align > begin_align);

    /* Initialize system memory heap */
    _MEM_INIT("heap", begin_addr, end_align - begin_align);
}

/**
 * @brief Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 *
 * @return the pointer to allocated memory or NULL if no free memory was found.
 */
fm_weak void *fm_malloc(fm_size_t size)
{
    void *ptr;

    /* allocate memory block from system heap */
    ptr = _MEM_MALLOC(size);

    return ptr;
}

/**
 * @brief This function will change the size of previously allocated memory block.
 *
 * @param ptr is the pointer to memory allocated by fm_malloc.
 *
 * @param newsize is the required new size.
 *
 * @return the changed memory block address.
 */
fm_weak void *fm_realloc(void *ptr, fm_size_t newsize)
{
    void *nptr;

    /* Change the size of previously allocated memory block */
    nptr = _MEM_REALLOC(ptr, newsize);

    return nptr;
}

/**
 * @brief  This function will contiguously allocate enough space for count objects
 *         that are size bytes of memory each and returns a pointer to the allocated
 *         memory.
 *
 * @note   The allocated memory is filled with bytes of value zero.
 *
 * @param  count is the number of objects to allocate.
 *
 * @param  size is the size of one object to allocate.
 *
 * @return pointer to allocated memory / NULL pointer if there is an error.
 */
fm_weak void *fm_calloc(fm_size_t count, fm_size_t size)
{
    void *p;

    /* allocate 'count' objects of size 'size' */
    p = fm_malloc(count * size);
    /* zero the memory */
    if (p)
    {
        fm_memset(p, 0, count * size);
    }
    return p;
}

/**
 * @brief This function will release the previously allocated memory block by
 *        fm_malloc. The released memory block is taken back to system heap.
 *
 * @param ptr the address of memory which will be released.
 */
fm_weak void fm_free(void *ptr)
{
    /* NULL check */
    if (ptr == FM_NULL) return;

    _MEM_FREE(ptr);
}

/**
* @brief This function will caculate the total memory, the used memory, and
*        the max used memory.
*
* @param total is a pointer to get the total size of the memory.
*
* @param used is a pointer to get the size of memory used.
*
* @param max_used is a pointer to get the maximum memory used.
*/
fm_weak void fm_memory_info(fm_size_t *total,
                            fm_size_t *used,
                            fm_size_t *max_used)
{
    _MEM_INFO(total, used, max_used);
}

/**
 * @brief  This function allocates a memory block, which address is aligned to the
 * specified alignment size.
 *
 * @param  size is the allocated memory block size.
 *
 * @param  align is the alignment size.
 *
 * @return The memory block address was returned successfully, otherwise it was
 *         returned empty FM_NULL.
 */
fm_weak void *fm_malloc_align(fm_size_t size, fm_size_t align)
{
    void *ptr = FM_NULL;
    void *align_ptr = FM_NULL;
    int uintptr_size = 0;
    fm_size_t align_size = 0;

    /* sizeof pointer */
    uintptr_size = sizeof(void*);
    uintptr_size -= 1;

    /* align the alignment size to uintptr size byte */
    align = ((align + uintptr_size) & ~uintptr_size);

    /* get total aligned size */
    align_size = ((size + uintptr_size) & ~uintptr_size) + align;
    /* allocate memory block from heap */
    ptr = fm_malloc(align_size);
    if (ptr != FM_NULL)
    {
        /* the allocated memory block is aligned */
        if (((fm_ubase_t)ptr & (align - 1)) == 0)
        {
            align_ptr = (void *)((fm_ubase_t)ptr + align);
        }
        else
        {
            align_ptr = (void *)(((fm_ubase_t)ptr + (align - 1)) & ~(align - 1));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((fm_ubase_t *)((fm_ubase_t)align_ptr - sizeof(void *))) = (fm_ubase_t)ptr;

        ptr = align_ptr;
    }

    return ptr;
}

/**
 * @brief This function release the memory block, which is allocated by
 * fm_malloc_align function and address is aligned.
 *
 * @param ptr is the memory block pointer.
 */
fm_weak void fm_free_align(void *ptr)
{
    void *real_ptr = FM_NULL;

    /* NULL check */
    if (ptr == FM_NULL) return;
    real_ptr = (void *) * (fm_ubase_t *)((fm_ubase_t)ptr - sizeof(void *));
    fm_free(real_ptr);
}

/* hardware control */
fm_weak fm_base_t fm_hw_interrupt_disable(void)
{
    return 0;
}

fm_weak void fm_hw_interrupt_enable(fm_base_t level)
{
    (void)(level);
}

fm_weak void fm_hw_console_output(const char *str)
{
    (void)(str);
}

#endif /* FM_USING_HEAP */