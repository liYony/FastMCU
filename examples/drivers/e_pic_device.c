#include <fmdevice.h>

#define DBG_TAG "e.pic.device"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

struct fm_resource virt_intc_resources[] =
{
    [0] = {
        .name = "irq0",
        .start = 20,
        .end = 20,
        .flags = FM_IORESOURCE_IRQ | FM_IORESOURCE_IRQ_HIGHLEVEL,
    },
    [1] = {
        .name = "irq1",
        .start = 21,
        .end = 21,
        .flags = FM_IORESOURCE_IRQ | FM_IORESOURCE_IRQ_HIGHLEVEL,
    },
    [2] = {
        .name = "irq2",
        .start = 22,
        .end = 22,
        .flags = FM_IORESOURCE_IRQ | FM_IORESOURCE_IRQ_HIGHLEVEL,
    },
    [3] = {
        .name = "irq3",
        .start = 23,
        .end = 23,
        .flags = FM_IORESOURCE_IRQ | FM_IORESOURCE_IRQ_HIGHLEVEL,
    }
};

struct fm_platform_device *virt_intc_device = FM_NULL;

static int e_platform_device_init(void)
{
    virt_intc_device = fm_platform_device_alloc("virt_device");

    virt_intc_device->resource = virt_intc_resources;
    virt_intc_device->num_resources = FM_ARRAY_SIZE(virt_intc_resources);

    fm_platform_device_register(virt_intc_device);

    return 0;
}

INIT_DEVICE_EXPORT(e_platform_device_init);
