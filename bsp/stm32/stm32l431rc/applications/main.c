/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-12-19     liYony       first version
 */

#include <fmcu.h>

int main(void)
{
    fmcu_startup(); 
    fm_kprintf("FastMCU app running.\r\n");
    while(1)
    {
    }
}
