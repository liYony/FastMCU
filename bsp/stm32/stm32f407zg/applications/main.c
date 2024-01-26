/*
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-01-26     liYony       first version
 */

#include <fmcu.h>

int main(void)
{
    fmcu_setup(); 
    while(1)
    {
        fmcu_loop();
    }
}
