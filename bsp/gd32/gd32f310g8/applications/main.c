#include <qk_section.h>
#include <qk_halport.h>

int main(void)
{
    qk_init();
    while(1)
    {
        qk_exec();
    }
    return 0;
}

void while_run(void)
{
    qk_delay_ms(1000);
    printf("123456\r\n");
}

POLLING_EXPORT(while_run);