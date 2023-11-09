#include <fm_section.h>
#include <dal.h>

int main(void)
{
    dal_init();
    fm_section_init();
    log_i("HC32F4A0 RUN....");
    while(1)
    {
        fm_section_exec();
//        dal_delay_ms(1000);
//        log_d("test");
    }
}
