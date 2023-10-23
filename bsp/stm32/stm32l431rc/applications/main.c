#include <qk_section.h>
#include <qk_halport.h>

int main(void)
{
    qk_hal_init();
    qk_init();
    while(1)
    {
        qk_exec();
//        qk_delay_ms(1000);
    }
}
