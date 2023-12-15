#ifndef __FM_DEF_H__
#define __FM_DEF_H__

#include <fmconfig.h>
#define FM_USING_LIBC
#define FM_NAME_MAX 20
#ifdef FM_USING_LIBC
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
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

/* RT-Thread error code definitions */
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

typedef struct fm_device *fm_device_t;

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
    char                      name[FM_NAME_MAX];        /**< device name */
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

    fm_list_t                 list; 
    void                     *user_data;                /**< device private data */
};

#ifdef __cplusplus
}
#endif

#endif
