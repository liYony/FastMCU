#include "nr_micro_shell.h"
#include <string.h>
#include <ctype.h>
#include <fm_section.h>

void fm_nr_shell_init()
{
    shell_init();
}

INITLV4_EXPORT(fm_nr_shell_init);

void fm_nr_shell_exec(void)
{
    uint8_t c;
    if(fm_getchar(&c) == 0)
    {
        shell(c);
    }
}

POLLING_EXPORT(fm_nr_shell_exec);
