#include "nr_micro_shell.h"
#include <string.h>
#include <ctype.h>
#include <fmcu.h>

int fm_nr_shell_init()
{
    shell_init();

    return 0;
}

INIT_ENV_EXPORT(fm_nr_shell_init);

static int fm_nr_shell_getchar(const char *c)
{
    fm_device_t c_dev = fm_console_get_device();

    return fm_device_read(c_dev, 0, (void *)c, 1);
}

int fm_nr_shell_exec(void)
{
    char c;
    if (fm_nr_shell_getchar(&c) == 1)
    {
        shell(c);
    }

    return 0;
}

INIT_APP_EXPORT(fm_nr_shell_exec);
