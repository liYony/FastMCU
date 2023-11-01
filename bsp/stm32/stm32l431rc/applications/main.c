#include <fm_section.h>

#include <dal.h>

int main(void)
{
    dal_init();
    fm_section_init();
    while(1)
    {
        fm_section_exec();
//        dal_delay_ms(1000);
    }
}
