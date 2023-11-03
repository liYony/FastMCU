#include <fm_section.h>
#include <dal.h>

int main(void)
{
    dal_init();
    fm_section_init();
    log_d("STM32G030 RUNNING...");
    while(1)
    {
        fm_section_exec();
    }
}
