/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2007-01-10     Bernard      the first version
 * 2008-07-12     Bernard      remove all rt_int8, rt_uint32_t etc typedef
 * 2010-10-26     yi.qiu       add module support
 * 2010-11-10     Bernard      add cleanup callback function in thread exit.
 * 2011-05-09     Bernard      use builtin va_arg in GCC 4.x
 * 2012-11-16     Bernard      change RT_NULL from ((void*)0) to 0.
 * 2012-12-29     Bernard      change the RT_USING_MEMPOOL location and add
 *                             RT_USING_MEMHEAP condition.
 * 2012-12-30     Bernard      add more control command for graphic.
 * 2013-01-09     Bernard      change version number.
 * 2015-02-01     Bernard      change version number to v2.1.0
 * 2017-08-31     Bernard      change version number to v3.0.0
 * 2017-11-30     Bernard      change version number to v3.0.1
 * 2017-12-27     Bernard      change version number to v3.0.2
 * 2018-02-24     Bernard      change version number to v3.0.3
 * 2018-04-25     Bernard      change version number to v3.0.4
 * 2018-05-31     Bernard      change version number to v3.1.0
 * 2018-09-04     Bernard      change version number to v3.1.1
 * 2018-09-14     Bernard      apply Apache License v2.0 to RT-Thread Kernel
 * 2018-10-13     Bernard      change version number to v4.0.0
 * 2018-10-02     Bernard      add 64bit arch support
 * 2018-11-22     Jesven       add smp member to struct rt_thread
 *                             add struct rt_cpu
 *                             add smp relevant macros
 * 2019-01-27     Bernard      change version number to v4.0.1
 * 2019-05-17     Bernard      change version number to v4.0.2
 * 2019-12-20     Bernard      change version number to v4.0.3
 * 2020-08-10     Meco Man     add macro for struct rt_device_ops
 * 2020-10-23     Meco Man     define maximum value of ipc type
 * 2021-03-19     Meco Man     add security devices
 * 2021-05-10     armink       change version number to v4.0.4
 * 2021-11-19     Meco Man     change version number to v4.1.0
 * 2021-12-21     Meco Man     re-implement RT_UNUSED
 * 2022-01-01     Gabriel      improve hooking method
 * 2022-01-07     Gabriel      move some __on_rt_xxxxx_hook to dedicated c source files
 * 2022-01-12     Meco Man     remove RT_THREAD_BLOCK
 * 2022-04-20     Meco Man     change version number to v4.1.1
 * 2022-04-21     THEWON       add macro RT_VERSION_CHECK
 * 2022-06-29     Meco Man     add RT_USING_LIBC and standard libc headers
 * 2022-08-16     Meco Man     change version number to v5.0.0
 * 2022-09-12     Meco Man     define rt_ssize_t
 * 2022-12-20     Meco Man     add const name for rt_object
 * 2023-04-01     Chushicheng  change version number to v5.0.1
 * 2023-05-20     Bernard      add stdc atomic detection.
 * 2023-09-15     xqyjlj       perf rt_hw_interrupt_disable/enable
 * 2023-10-10     Chushicheng  change version number to v5.1.0
 * 2023-10-11     zmshahaha    move specific devices related and driver to components/drivers
 * 2023-11-21     Meco Man     add RT_USING_NANO macro
 * 2023-12-19     liYony       Adaptive FastMCU
 * 2023-12-29     liYony       add Finite state machine
 */

#ifndef __FM_DEF_H__
#define __FM_DEF_H__

#include <fmconfig.h>
#ifdef FM_USING_LIBC
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#endif /* FM_USING_LIBC */

#ifdef __cplusplus
extern "C" {
#endif

/* FastMCU version information */
#define FM_VERSION_MAJOR                1               /**< Major version number (X.x.x) */
#define FM_VERSION_MINOR                0               /**< Minor version number (x.X.x) */
#define FM_VERSION_PATCH                0               /**< Patch version number (x.x.X) */

/* e.g. #if (FMCU_VERSION >= FM_VERSION_CHECK(4, 1, 0) */
#define FM_VERSION_CHECK(major, minor, revise)          ((major * 10000) + (minor * 100) + revise)

/* FastMCU version */
#define FMCU_VERSION                FM_VERSION_CHECK(FM_VERSION_MAJOR, FM_VERSION_MINOR, FM_VERSION_PATCH)

/* FastMCU basic data type definitions */
typedef int                             fm_bool_t;      /**< boolean type */
typedef signed long                     fm_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   fm_ubase_t;     /**< Nbit unsigned CPU related data type */

#ifdef FM_USING_LIBC
typedef int8_t                          fm_int8_t;      /**<  8bit integer type */
typedef int16_t                         fm_int16_t;     /**< 16bit integer type */
typedef int32_t                         fm_int32_t;     /**< 32bit integer type */
typedef uint8_t                         fm_uint8_t;     /**<  8bit unsigned integer type */
typedef uint16_t                        fm_uint16_t;    /**< 16bit unsigned integer type */
typedef uint32_t                        fm_uint32_t;    /**< 32bit unsigned integer type */
typedef int64_t                         fm_int64_t;     /**< 64bit integer type */
typedef uint64_t                        fm_uint64_t;    /**< 64bit unsigned integer type */
typedef size_t                          fm_size_t;      /**< Type for size number */
typedef fm_base_t                       fm_ssize_t;     /**< Used for a count of bytes or an error indication */
#else
typedef signed   char                   fm_int8_t;      /**<  8bit integer type */
typedef signed   short                  fm_int16_t;     /**< 16bit integer type */
typedef signed   int                    fm_int32_t;     /**< 32bit integer type */
typedef unsigned char                   fm_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  fm_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned int                    fm_uint32_t;    /**< 32bit unsigned integer type */
#ifdef ARCH_CPU_64BIT
typedef signed long                     fm_int64_t;     /**< 64bit integer type */
typedef unsigned long                   fm_uint64_t;    /**< 64bit unsigned integer type */
#else
typedef signed long long                fm_int64_t;     /**< 64bit integer type */
typedef unsigned long long              fm_uint64_t;    /**< 64bit unsigned integer type */
#endif /* ARCH_CPU_64BIT */
typedef fm_ubase_t                      fm_size_t;      /**< Type for size number */
typedef fm_base_t                       fm_ssize_t;     /**< Used for a count of bytes or an error indication */
#endif /* FM_USING_LIBC */

typedef fm_base_t                       fm_err_t;       /**< Type for error number */
typedef fm_uint32_t                     fm_time_t;      /**< Type for time stamp */
typedef fm_uint32_t                     fm_tick_t;      /**< Type for tick count */
typedef fm_base_t                       fm_flag_t;      /**< Type for flags */
typedef fm_ubase_t                      fm_dev_t;       /**< Type for device */
typedef fm_base_t                       fm_off_t;       /**< Type for offset */

/* boolean type definitions */
#define FM_TRUE                         1               /**< boolean true  */
#define FM_FALSE                        0               /**< boolean fails */

/* null pointer definition */
#define FM_NULL                         0

/* maximum value of base type */
#ifdef FM_USING_LIBC
#define FM_UINT8_MAX                    UINT8_MAX       /**< Maximum number of UINT8 */
#define FM_UINT16_MAX                   UINT16_MAX      /**< Maximum number of UINT16 */
#define FM_UINT32_MAX                   UINT32_MAX      /**< Maximum number of UINT32 */
#else
#define FM_UINT8_MAX                    0xff            /**< Maximum number of UINT8 */
#define FM_UINT16_MAX                   0xffff          /**< Maximum number of UINT16 */
#define FM_UINT32_MAX                   0xffffffff      /**< Maximum number of UINT32 */
#endif /* FM_USING_LIBC */

/* Compiler Related Definitions */
#if defined(__ARMCC_VERSION)        /* ARM Compiler */
#define fm_section(x)               __attribute__((section(x)))
#define fm_used                     __attribute__((used))
#define fm_align(n)                 __attribute__((aligned(n)))
#define fm_weak                     __attribute__((weak))
#define fm_inline                   static __inline
#elif defined (__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define fm_section(x)               @ x
#define fm_used                     __root
#define PRAGMA(x)                   _Pragma(#x)
#define fm_align(n)                 PRAGMA(data_alignment=n)
#define fm_weak                     __weak
#define fm_inline                   static inline
#elif defined (__GNUC__)            /* GNU GCC Compiler */
#ifndef FM_USING_LIBC
/* the version of GNU GCC must be greater than 4.x */
typedef __builtin_va_list           __gnuc_va_list;
typedef __gnuc_va_list              va_list;
#define va_start(v,l)               __builtin_va_start(v,l)
#define va_end(v)                   __builtin_va_end(v)
#define va_arg(v,l)                 __builtin_va_arg(v,l)
#endif /* FM_USING_LIBC */
#define __FM_STRINGIFY(x...)        #x
#define FM_STRINGIFY(x...)          __FM_STRINGIFY(x)
#define fm_section(x)               __attribute__((section(x)))
#define fm_used                     __attribute__((used))
#define fm_align(n)                 __attribute__((aligned(n)))
#define fm_weak                     __attribute__((weak))
#define fm_inline                   static __inline
#elif defined (__ADSPBLACKFIN__)        /* for VisualDSP++ Compiler */
#define fm_section(x)               __attribute__((section(x)))
#define fm_used                     __attribute__((used))
#define fm_align(n)                 __attribute__((aligned(n)))
#define fm_weak                     __attribute__((weak))
#define fm_inline                   static inline
#elif defined (_MSC_VER)
#define fm_section(x)
#define fm_used
#define fm_align(n)                 __declspec(align(n))
#define fm_weak
#define fm_inline                   static __inline
#elif defined (__TI_COMPILER_VERSION__)
/* The way that TI compiler set section is different from other(at least
    * GCC and MDK) compilers. See ARM Optimizing C/C++ Compiler 5.9.3 for more
    * details. */
#define fm_section(x)               __attribute__((section(x)))
#ifdef __TI_EABI__
#define fm_used                     __attribute__((retain)) __attribute__((used))
#else
#define fm_used                     __attribute__((used))
#endif
#define PRAGMA(x)                   _Pragma(#x)
#define fm_align(n)                 __attribute__((aligned(n)))
#ifdef __TI_EABI__
#define fm_weak                     __attribute__((weak))
#else
#define fm_weak
#endif
#define fm_inline                   static inline
#elif defined (__TASKING__)
#define fm_section(x)               __attribute__((section(x)))
#define fm_used                     __attribute__((used, protect))
#define PRAGMA(x)                   _Pragma(#x)
#define fm_align(n)                 __attribute__((__align(n)))
#define fm_weak                     __attribute__((weak))
#define fm_inline                   static inline
#else
    #error not supported tool chain
#endif /* __ARMCC_VERSION */

/* FastMCU error code definitions */
#define FM_EOK                          0               /**< There is no error */
#define FM_ERROR                        1               /**< A generic error happens */
#define FM_ETIMEOUT                     2               /**< Timed out */
#define FM_EFULL                        3               /**< The resource is full */
#define FM_EEMPTY                       4               /**< The resource is empty */
#define FM_ENOMEM                       5               /**< No memory */
#define FM_ENOSYS                       6               /**< No system */
#define FM_EBUSY                        7               /**< Busy */
#define FM_EIO                          8               /**< IO error */
#define FM_EINTR                        9               /**< Interrupted system call */
#define FM_EINVAL                       10              /**< Invalid argument */
#define FM_ETRAP                        11              /**< Trap event */
#define FM_ENOENT                       12              /**< No entry */
#define FM_ENOSPC                       13              /**< No space left */

/**
 * @ingroup BasicDef
 *
 * @def FM_IS_ALIGN(addr, align)
 * Return true(1) or false(0).
 *     FM_IS_ALIGN(128, 4) is judging whether 128 aligns with 4.
 *     The result is 1, which means 128 aligns with 4.
 * @note If the address is NULL, false(0) will be returned
 */
#define FM_IS_ALIGN(addr, align) ((!(addr & (align - 1))) && (addr != FM_NULL))

/**
 * @ingroup BasicDef
 *
 * @def FM_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. FM_ALIGN(13, 4)
 * would return 16.
 */
#define FM_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def FM_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. FM_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define FM_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define FM_MS2TICKS(m)                  (m / (1000 / FM_TICK_PER_SECOND))
#define FM_TICKS2MS(t)                  (t * (1000 / FM_TICK_PER_SECOND))

#define FM_WAITING_FOREVER              -1              /**< Block forever until get resource. */
#define FM_WAITING_NO                   0               /**< Non-block. */

/**
 * @brief This macro function asserts a condition.
 *
 * @param EX The condition to assert.
 */
#ifdef FM_USING_ASSERT
#define FM_ASSERT(EX)                                                               \
    do{                                                                             \
        if (!(EX))                                                                  \
        {                                                                           \
            fm_kprintf("(%s) assertion failed at function:%s, line number:%d \r\n", \
                        #EX, (__FUNCTION__), (__LINE__));                           \
            while(1);                                                               \
        }                                                                           \
    } while(0)
#else
#define FM_ASSERT(EX)
#endif /* FM_USING_ASSERT */

/* initialization export */
#ifdef FM_USING_COMPONENTS_INIT
typedef int (*init_fn_t)(void);
#ifdef _MSC_VER
#pragma section("fmi_fn$f",read)
    #ifdef FM_DEBUGING_INIT
        struct fm_init_desc
        {
            const char* level;
            const init_fn_t fn;
            const char* fn_name;
        };
        #define INIT_EXPORT(fn, level)                                  \
                                const char __fmi_level_##fn[] = ".fmi_fn." level;       \
                                const char __fmi_##fn##_name[] = #fn;                   \
                                __declspec(allocate("fmi_fn$f"))                        \
                                fm_used const struct fm_init_desc __fm_init_msc_##fn =  \
                                {__fmi_level_##fn, fn, __fmi_##fn##_name};
    #else
        struct fm_init_desc
        {
            const char* level;
            const init_fn_t fn;
        };
        #define INIT_EXPORT(fn, level)                                  \
                                const char __fmi_level_##fn[] = ".fmi_fn." level;       \
                                __declspec(allocate("fmi_fn$f"))                        \
                                fm_used const struct fm_init_desc __fm_init_msc_##fn =  \
                                {__fmi_level_##fn, fn };
    #endif
#else
    #ifdef FM_DEBUGING_INIT
        struct fm_init_desc
        {
            const char* fn_name;
            const init_fn_t fn;
        };
        #define INIT_EXPORT(fn, level)                                                       \
            const char __fmi_##fn##_name[] = #fn;                                            \
            fm_used const struct fm_init_desc __fm_init_desc_##fn fm_section(".fmi_fn." level) = \
            { __fmi_##fn##_name, fn};
    #else
        #define INIT_EXPORT(fn, level)                                                       \
            fm_used const init_fn_t __fm_init_##fn fm_section(".fmi_fn." level) = fn
    #endif
#endif
#else
#define INIT_EXPORT(fn, level)
#endif

/* board init routines will be called in board_init() function */
#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, "1")

/* init cpu, memory, interrupt-controller, bus... */
#define INIT_CORE_EXPORT(fn)            INIT_EXPORT(fn, "1.0")
/* init pci/pcie, usb platform driver... */
#define INIT_FRAMEWORK_EXPORT(fn)       INIT_EXPORT(fn, "1.1")
/* init platform, user code... */
#define INIT_PLATFORM_EXPORT(fn)        INIT_EXPORT(fn, "1.2")
/* init sys-timer, clk, pinctrl... */
#define INIT_SUBSYS_EXPORT(fn)          INIT_EXPORT(fn, "1.3")
/* init early drivers */
#define INIT_DRIVER_EARLY_EXPORT(fn)    INIT_EXPORT(fn, "1.4")

/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initialization) */
#define INIT_PREV_EXPORT(fn)            INIT_EXPORT(fn, "2")
/* device initialization */
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, "3")
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, "4")
/* environment initialization (mount disk, ...) */
#define INIT_ENV_EXPORT(fn)             INIT_EXPORT(fn, "5")
/* application initialization (rtgui application etc ...) */
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, "6")

#define FM_SAFE_INVOKE(f, ...)  \
    do                          \
    {                           \
        if ((f) != NULL)        \
        {                       \
            (f)(__VA_ARGS__);   \
        }                       \
    } while (0)

/* kernel malloc definitions */
#ifndef FM_KERNEL_MALLOC
#define FM_KERNEL_MALLOC(sz)            fm_malloc(sz)
#endif

#ifndef FM_KERNEL_FREE
#define FM_KERNEL_FREE(ptr)             fm_free(ptr)
#endif

#ifndef FM_KERNEL_REALLOC
#define FM_KERNEL_REALLOC(ptr, size)    fm_realloc(ptr, size)
#endif

/**
 * Double List structure
 */
struct fm_list_node
{
    struct fm_list_node *next;                          /**< point to next node. */
    struct fm_list_node *prev;                          /**< point to prev node. */
};
typedef struct fm_list_node fm_list_t;                  /**< Type for lists. */

/**
 * Single List structure
 */
struct fm_slist_node
{
    struct fm_slist_node *next;                         /**< point to next node. */
};
typedef struct fm_slist_node fm_slist_t;                /**< Type for single list. */

/**
 * Base structure of Kernel object
 */
struct fm_object
{
#if FM_NAME_MAX > 0
    char        name[FM_NAME_MAX];                       /**< dynamic name of kernel object */
#else
    const char *name;                                    /**< static name of kernel object */
#endif /* FM_NAME_MAX > 0 */
    fm_uint8_t  type;                                    /**< type of kernel object */

    fm_list_t   list;                                    /**< list node of kernel object */
};
typedef struct fm_object *fm_object_t;                   /**< Type for kernel objects. */

enum fm_object_class_type
{
    FM_Object_Class_Null          = 0x00,      /**< The object is not used. */
    FM_Object_Class_Device        = 0x01,      /**< The object is a device. */
    FM_Object_Class_Fsm           = 0x02,      /**< The object is a fsm. */
    FM_Object_Class_Timer         = 0x03,      /**< The object is a timer. */
    FM_Object_Class_Unknown       = 0x04,      /**< The object is unknown. */
    FM_Object_Class_Static        = 0x80       /**< The object is a static object. */
};

/**
 * The information of the kernel object
 */
struct fm_object_information
{
    enum fm_object_class_type type;                     /**< object class type */
    fm_list_t                 object_list;              /**< object list */
    fm_size_t                 object_size;              /**< object size */
};

/**
 * device (I/O) class type
 */
enum fm_device_class_type
{
    FM_Device_Class_Char = 0,                           /**< character device */
    FM_Device_Class_Block,                              /**< block device */
    FM_Device_Class_NetIf,                              /**< net interface */
    FM_Device_Class_MTD,                                /**< memory device */
    FM_Device_Class_CAN,                                /**< CAN device */
    FM_Device_Class_RTC,                                /**< RTC device */
    FM_Device_Class_Sound,                              /**< Sound device */
    FM_Device_Class_Graphic,                            /**< Graphic device */
    FM_Device_Class_I2CBUS,                             /**< I2C bus device */
    FM_Device_Class_USBDevice,                          /**< USB slave device */
    FM_Device_Class_USBHost,                            /**< USB host bus */
    FM_Device_Class_USBOTG,                             /**< USB OTG bus */
    FM_Device_Class_SPIBUS,                             /**< SPI bus device */
    FM_Device_Class_SPIDevice,                          /**< SPI device */
    FM_Device_Class_SDIO,                               /**< SDIO bus device */
    FM_Device_Class_PM,                                 /**< PM pseudo device */
    FM_Device_Class_Pipe,                               /**< Pipe device */
    FM_Device_Class_Portal,                             /**< Portal device */
    FM_Device_Class_Timer,                              /**< Timer device */
    FM_Device_Class_Miscellaneous,                      /**< Miscellaneous device */
    FM_Device_Class_Sensor,                             /**< Sensor device */
    FM_Device_Class_Touch,                              /**< Touch device */
    FM_Device_Class_PHY,                                /**< PHY device */
    FM_Device_Class_Security,                           /**< Security device */
    FM_Device_Class_WLAN,                               /**< WLAN device */
    FM_Device_Class_Pin,                                /**< Pin device */
    FM_Device_Class_ADC,                                /**< ADC device */
    FM_Device_Class_DAC,                                /**< DAC device */
    FM_Device_Class_WDT,                                /**< WDT device */
    FM_Device_Class_PWM,                                /**< PWM device */
    FM_Device_Class_Bus,                                /**< Bus device */
    FM_Device_Class_Unknown                             /**< unknown device */
};

/**
 * device flags definitions
 */
#define FM_DEVICE_FLAG_DEACTIVATE       0x000           /**< device is not not initialized */

#define FM_DEVICE_FLAG_RDONLY           0x001           /**< read only */
#define FM_DEVICE_FLAG_WRONLY           0x002           /**< write only */
#define FM_DEVICE_FLAG_RDWR             0x003           /**< read and write */

#define FM_DEVICE_FLAG_REMOVABLE        0x004           /**< removable device */
#define FM_DEVICE_FLAG_STANDALONE       0x008           /**< standalone device */
#define FM_DEVICE_FLAG_ACTIVATED        0x010           /**< device is activated */
#define FM_DEVICE_FLAG_SUSPENDED        0x020           /**< device is suspended */
#define FM_DEVICE_FLAG_STREAM           0x040           /**< stream mode */

#define FM_DEVICE_FLAG_INT_RX           0x100           /**< INT mode on Rx */
#define FM_DEVICE_FLAG_DMA_RX           0x200           /**< DMA mode on Rx */
#define FM_DEVICE_FLAG_INT_TX           0x400           /**< INT mode on Tx */
#define FM_DEVICE_FLAG_DMA_TX           0x800           /**< DMA mode on Tx */

#define FM_DEVICE_OFLAG_CLOSE           0x000           /**< device is closed */
#define FM_DEVICE_OFLAG_RDONLY          0x001           /**< read only access */
#define FM_DEVICE_OFLAG_WRONLY          0x002           /**< write only access */
#define FM_DEVICE_OFLAG_RDWR            0x003           /**< read and write */
#define FM_DEVICE_OFLAG_OPEN            0x008           /**< device is opened */
#define FM_DEVICE_OFLAG_MASK            0xf0f           /**< mask of open flag */

/**
 * general device commands
 * 0x01 - 0x1F general device control commands
 * 0x20 - 0x3F udevice control commands
 * 0x40 -      special device control commands
 */
#define FM_DEVICE_CTRL_RESUME           0x01            /**< resume device */
#define FM_DEVICE_CTRL_SUSPEND          0x02            /**< suspend device */
#define FM_DEVICE_CTRL_CONFIG           0x03            /**< configure device */
#define FM_DEVICE_CTRL_CLOSE            0x04            /**< close device */
#define FM_DEVICE_CTRL_NOTIFY_SET       0x05            /**< set notify func */
#define FM_DEVICE_CTRL_SET_INT          0x06            /**< set interrupt */
#define FM_DEVICE_CTRL_CLR_INT          0x07            /**< clear interrupt */
#define FM_DEVICE_CTRL_GET_INT          0x08            /**< get interrupt status */
#define FM_DEVICE_CTRL_CONSOLE_OFLAG    0x09            /**< get console open flag */
#define FM_DEVICE_CTRL_MASK             0x1f            /**< mask for contrl commands */

/**
 * device control
 */
#define FM_DEVICE_CTRL_BASE(Type)        ((FM_Device_Class_##Type + 1) * 0x100)

/**
 * special device commands
 */
/* character device */
#define FM_DEVICE_CTRL_CHAR_STREAM      (FM_DEVICE_CTRL_BASE(Char) + 1)             /**< stream mode on char device */
/* block device */
#define FM_DEVICE_CTRL_BLK_GETGEOME     (FM_DEVICE_CTRL_BASE(Block) + 1)            /**< get geometry information   */
#define FM_DEVICE_CTRL_BLK_SYNC         (FM_DEVICE_CTRL_BASE(Block) + 2)            /**< flush data to block device */
#define FM_DEVICE_CTRL_BLK_ERASE        (FM_DEVICE_CTRL_BASE(Block) + 3)            /**< erase block on block device */
#define FM_DEVICE_CTRL_BLK_AUTOREFRESH  (FM_DEVICE_CTRL_BASE(Block) + 4)            /**< block device : enter/exit auto refresh mode */
#define FM_DEVICE_CTRL_BLK_PARTITION    (FM_DEVICE_CTRL_BASE(Block) + 5)            /**< get block device partition */
/* net interface device*/
#define FM_DEVICE_CTRL_NETIF_GETMAC     (FM_DEVICE_CTRL_BASE(NetIf) + 1)            /**< get mac address */
/* mtd interface device*/
#define FM_DEVICE_CTRL_MTD_FORMAT       (FM_DEVICE_CTRL_BASE(MTD) + 1)              /**< format a MTD device */

typedef struct fm_device *fm_device_t;

#ifdef FM_USING_DEVICE_OPS
/**
 * operations set for device object
 */
struct fm_device_ops
{
    /* common device interface */
    fm_err_t  (*init)   (fm_device_t dev);
    fm_err_t  (*open)   (fm_device_t dev, fm_uint16_t oflag);
    fm_err_t  (*close)  (fm_device_t dev);
    fm_ssize_t (*read)  (fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size);
    fm_ssize_t (*write) (fm_device_t dev, fm_off_t pos, const void *buffer, fm_size_t size);
    fm_err_t  (*control)(fm_device_t dev, int cmd, void *args);
};
#endif /* FM_USING_DEVICE_OPS */

/**
 * Device structure
 */
struct fm_device
{
    struct fm_object          parent;                   /**< inherit from fm_object */

    enum fm_device_class_type type;                     /**< device type */
    fm_uint16_t               flag;                     /**< device flag */
    fm_uint16_t               open_flag;                /**< device open flag */

    fm_uint8_t                ref_count;                /**< reference count */
    fm_uint8_t                device_id;                /**< 0 - 255 */

    /* device call back */
    fm_err_t (*rx_indicate)(fm_device_t dev, fm_size_t size);
    fm_err_t (*tx_complete)(fm_device_t dev, void *buffer);

#ifdef FM_USING_DEVICE_OPS
    const struct fm_device_ops *ops;
#else
    /* common device interface */
    fm_err_t  (*init)   (fm_device_t dev);
    fm_err_t  (*open)   (fm_device_t dev, fm_uint16_t oflag);
    fm_err_t  (*close)  (fm_device_t dev);
    fm_ssize_t (*read)  (fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size);
    fm_ssize_t (*write) (fm_device_t dev, fm_off_t pos, const void *buffer, fm_size_t size);
    fm_err_t  (*control)(fm_device_t dev, int cmd, void *args);
#endif /* FM_USING_DEVICE_OPS */

    void                     *user_data;                /**< device private data */
};

#ifdef FM_USING_MEMHEAP
/**
 * memory item on the heap
 */
struct fm_memheap_item
{
    fm_uint32_t             magic;                      /**< magic number for memheap */
    struct fm_memheap      *pool_ptr;                   /**< point of pool */

    struct fm_memheap_item *next;                       /**< next memheap item */
    struct fm_memheap_item *prev;                       /**< prev memheap item */

    struct fm_memheap_item *next_free;                  /**< next free memheap item */
    struct fm_memheap_item *prev_free;                  /**< prev free memheap item */
};

/**
 * Base structure of memory heap object
 */
struct fm_memheap
{
    char                    name[FM_NAME_MAX];          /**< pool name */
    void                   *start_addr;                 /**< pool start address and size */

    fm_size_t               pool_size;                  /**< pool size */
    fm_size_t               available_size;             /**< available size */
    fm_size_t               max_used_size;              /**< maximum allocated size */

    struct fm_memheap_item *block_list;                 /**< used block list */

    struct fm_memheap_item *free_list;                  /**< free block list */
    struct fm_memheap_item  free_header;                /**< free block list header */
};
#endif /* FM_USING_MEMHEAP */

/* ring buffer */
struct fm_ringbuffer
{
    fm_uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     */

    fm_uint32_t read_mirror : 1;
    fm_uint32_t read_index : 31;
    fm_uint32_t write_mirror : 1;
    fm_uint32_t write_index : 31;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    fm_int32_t buffer_size;
};

enum fm_ringbuffer_state
{
    FM_RINGBUFFER_EMPTY,
    FM_RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    FM_RINGBUFFER_HALFFULL,
};

/**
 * Completion
 */
struct fm_completion
{
    fm_uint32_t flag;
};

/**
 * Finite state machine
 */
#define FM_STATE_NONE ((fm_uint32_t)(-1))

typedef void (*s_event_fn_t)(fm_uint32_t event, void *arg);
typedef void (*s_enter_fn_t)(fm_uint32_t pre_state);
typedef void (*s_exit_fn_t)(void);
typedef void (*s_exec_fn_t)(void);

struct s_event
{
    fm_uint32_t event;
    s_event_fn_t fn_event;

    fm_slist_t list; /* s_event list */
};
typedef struct s_event *s_event_t;

struct s_info
{
    fm_uint32_t state;
    s_enter_fn_t fn_enter;
    s_exit_fn_t fn_exit;
    s_exec_fn_t fn_exec;
    
    fm_slist_t e_list; /* s_event list */
    fm_slist_t list; /* s_info list */
};
typedef struct s_info *s_info_t;

struct fsm_info
{
    char name[FM_NAME_MAX];
    s_info_t info;

    fm_slist_t s_list; /* s_info list */
    fm_slist_t list; /* fsm_info list */
};
typedef struct fsm_info *fsm_info_t;

#define FM_TIMER_FLAG_ONE_SHOT 0x0 /**< one shot timer */
#define FM_TIMER_FLAG_PERIODIC 0x1 /**< periodic timer */

/**
 * timer structure
 */
struct fm_timer
{
    char name[FM_NAME_MAX];

    void (*timeout_func)(void *parameter);
    void *parameter;

    fm_bool_t enable;
    fm_uint32_t tick;
    fm_uint32_t cycle;
    fm_uint8_t type;
    fm_slist_t list;
};
typedef struct fm_timer *fm_timer_t;

#ifdef __cplusplus
}
#endif

#endif
