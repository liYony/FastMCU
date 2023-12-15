#include <fm_section.h>
#define DBG_LEVEL         DBG_LOG
#include <fm_log.h>
int main(void)
{
    fm_section_init();
    log_i("start");
    while(1)
    {
        fm_section_exec();
    }
}
