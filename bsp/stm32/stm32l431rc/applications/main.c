/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-01-02     liYony       first version
 */

#include <fmcu.h>

int main(void)
{
    fmcu_setup(); 
    fm_kprintf("This is stm32l431rc.\n");
    while(1)
    {
        fmcu_loop();
    }
}
