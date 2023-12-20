#ifndef __DRV_TMRA_H__
#define __DRV_TMRA_H__

#include <fmcu.h>
#include <fmdevice.h>
#include <board.h>
#include "drv_irq.h"

struct hc32_tim_tmra
{
    struct fm_hwtimer_device    device;
    CM_TMRA_TypeDef             *instance;
    func_ptr_t                  irq_callback;
    struct hc32_irq_config      irq_config;
    char                        *name;
};

#endif /* __DRV_TMRA_H__ */
