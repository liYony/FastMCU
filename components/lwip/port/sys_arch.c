#include <arch/cc.h>
#include <dal.h>

u32_t sys_now(void)
{
    return (dal_get_systick());
}
