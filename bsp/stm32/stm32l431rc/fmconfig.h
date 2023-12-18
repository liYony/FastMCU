#ifndef __FMCU_CONFIG_H__
#define __FMCU_CONFIG_H__

/* Generated by FMCU Configuration */

/* FMCU Kernel Configuration */

#define FM_NAME_MAX 16
#define FM_TICK_PER_SECOND 1000
#define FM_USING_COMPONENTS_INIT
#define FM_ALIGN_SIZE 8
#define FM_USING_DEBUG
#define FM_DEBUGING_COLOR
#define FM_DEBUGING_INIT

/* Memory Management */

#define FM_USING_MEMHEAP
#define FM_MEMHEAP_BEST_MODE
#define FM_USING_MEMHEAP_AS_HEAP
#define FM_USING_HEAP

/* end of Memory Management */

/* Kernel Device Object */

#define FM_USING_DEVICE
#define FM_USING_CONSOLE
#define FM_CONSOLEBUF_SIZE 256
#define FM_CONSOLE_DEVICE_NAME "uart1"

/* end of Kernel Device Object */

/* end of FMCU Kernel Configuration */

/* FMCU Drivers Configuration */

#define FM_USING_DEVICE_DRIVERS
#define FM_USING_SERIAL
#define FM_SERIAL_USING_DMA

/* end of FMCU Drivers Configuration */

/* FMCU Components Configuration */


/* Network */


/* end of Network */

/* end of FMCU Components Configuration */
#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32L4

/* Hardware Drivers Config */

#define SOC_STM32L431RC

/* Onboard Peripheral Drivers */

/* On-chip Peripheral Drivers */

#define BSP_USING_UART
#define BSP_USING_UART1
#define BSP_UART1_RX_USING_DMA
#define BSP_UART1_TX_USING_DMA
#define BSP_UART1_RX_BUFSIZE 256
#define BSP_UART1_TX_BUFSIZE 0

/* end of On-chip Peripheral Drivers */


/* Board extended module Drivers */


/* end of Hardware Drivers Config */

#endif /* __FMCU_CONFIG_H__ */
