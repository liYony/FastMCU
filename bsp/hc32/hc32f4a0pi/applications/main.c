#include <qk_section.h>
#include <qk_halport.h>
#include <dal.h>

int main(void)
{
    dal_init();
    qk_init();
    log_d("HC32F4A0 RUN....\r\n");
    while(1)
    {
        qk_exec();
//        dal_delay_ms(1000);
//        log_d("test\r\n");
    }
}
