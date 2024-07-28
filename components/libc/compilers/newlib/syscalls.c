/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-02-11     Meco Man     remove _gettimeofday_r() and _times_r()
 * 2021-02-13     Meco Man     re-implement exit() and abort()
 * 2021-02-21     Meco Man     improve and beautify syscalls
 * 2021-02-24     Meco Man     fix bug of _isatty_r()
 */

#include <reent.h>
#include <fmcu.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>

#define DBG_TAG    "newlib.syscalls"
#define DBG_LVL    DBG_INFO
#include <fmdbg.h>

#define _WARNING_NOT_IMPLEMENTED "This function is not implemented"

#ifdef FM_USING_HEAP /* Memory routine */
void *_malloc_r(struct _reent *ptr, size_t size)
{
    void* result;

    result = (void*)fm_malloc(size);
    if (result == FM_NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void* result;

    result = (void*)fm_realloc(old, newlen);
    if (result == FM_NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void* result;

    result = (void*)fm_calloc(size, len);
    if (result == FM_NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void _free_r(struct _reent *ptr, void *addr)
{
    fm_free(addr);
}

#else
void *_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
    LOG_E("Please enable FM_USING_HEAP");
    FM_ASSERT(0);
    return FM_NULL;
}
#endif /*FM_USING_HEAP*/

void __libc_init_array(void)
{
    /* we not use __libc init_aray to initialize C++ objects */
    /* __libc_init_array is ARM code, not Thumb; it will cause a hardfault. */
}

/* Reentrant versions of system calls.  */
#ifndef _REENT_ONLY
int *__errno(void)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    return FM_NULL;
}
#endif

int _getpid_r(struct _reent *ptr)
{
    return 0;
}

int _close_r(struct _reent *ptr, int fd)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

int _execve_r(struct _reent *ptr, const char * name, char *const *argv, char *const *env)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fork_r(struct _reent *ptr)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _isatty_r(struct _reent *ptr, int fd)
{
    if (fd >=0 && fd < 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int _kill_r(struct _reent *ptr, int pid, int sig)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _link_r(struct _reent *ptr, const char *old, const char *new)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

int _wait_r(struct _reent *ptr, int *status)
{
    ptr->_errno = ENOTSUP;
    return -1;
}

mode_t umask(mode_t mask)
{
    return 022;
}

int flock(int fd, int operation)
{
    return 0;
}

_off_t _lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

int _mkdir_r(struct _reent *ptr, const char *name, int mode)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

int _open_r(struct _reent *ptr, const char *file, int flags, int mode)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

_ssize_t _read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

int _rename_r(struct _reent *ptr, const char *old, const char *new)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

int _stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

int _unlink_r(struct _reent *ptr, const char *file)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

_ssize_t _write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
#if defined(FM_USING_CONSOLE) && defined(FM_USING_DEVICE)
        fm_device_t console;

        console = fm_console_get_device();
        if (console)
            return fm_device_write(console, -1, buf, nbytes);
#else
        ptr->_errno = ENOTSUP;
        return -1;
#endif /* defined(FM_USING_CONSOLE) && defined(FM_USING_DEVICE) */
    }
    else if (fd == STDIN_FILENO)
    {
        ptr->_errno = ENOTSUP;
        return -1;
    }

    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    ptr->_errno = ENOTSUP;
    return -1;
}

/* for exit() and abort() */
__attribute__ ((noreturn)) void _exit (int status)
{
    LOG_W("%s: %s", __func__, _WARNING_NOT_IMPLEMENTED);
    while(1);
}

/*
These functions are implemented and replaced by the 'common/time.c' file
int _gettimeofday_r(struct _reent *ptr, struct timeval *__tp, void *__tzp);
_CLOCK_T_  _times_r(struct _reent *ptr, struct tms *ptms);
*/
