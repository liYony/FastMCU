/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 * Copyright (c) 2022, Xiaohua Semiconductor Co., Ltd.
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-28     CDT          first version
 * 2023-12-18     liYony       adapt FastMCU
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <fmcu.h>
#include "hc32_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HC32_SRAM_SIZE                  (512)
#define HC32_SRAM_END                   (0x1FFE0000 + HC32_SRAM_SIZE * 1024)

#ifdef __ARMCC_VERSION
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define HEAP_BEGIN                      (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define HEAP_BEGIN                      (__segment_end("HEAP"))
#else
extern int __bss_end;
#define HEAP_BEGIN                      (&__bss_end)
#endif

#define HEAP_END                        HC32_SRAM_END

void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif
