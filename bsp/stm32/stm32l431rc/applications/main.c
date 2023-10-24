#include <qk_section.h>
#include <qk_halport.h>
#include <dal.h>

int main(void)
{
    dal_init();
    qk_init();
    while(1)
    {
        qk_exec();
//        dal_delay_ms(1000);
    }
}
