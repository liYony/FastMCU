#include <fmdevice.h>

#define DBG_TAG "e.pic.driver"
#define DBG_LVL DBG_INFO
#include <fmdbg.h>

static struct fm_pic_cell_args *pic_args;
static int trig_irq = -1;

fm_err_t virt_intc_irq_init(struct fm_pic *pic)
{
    fm_kprintf("%s\n", __func__);
    return FM_EOK;
}

fm_err_t virt_intc_irq_finit(struct fm_pic *pic)
{
    fm_kprintf("%s\n", __func__);
    return FM_EOK;
}

void virt_intc_irq_enable(struct fm_pic_irq *pirq)
{
    fm_kprintf("%s irq = %d\n", __func__, pirq->irq);
}

void virt_intc_irq_disable(struct fm_pic_irq *pirq)
{
    fm_kprintf("%s irq = %d\n", __func__, pirq->irq);
}

void virt_intc_irq_ack(struct fm_pic_irq *pirq)
{
    fm_kprintf("%s irq = %d\n", __func__, pirq->irq);
}

void virt_intc_irq_mask(struct fm_pic_irq *pirq)
{
    fm_kprintf("%s irq = %d\n", __func__, pirq->irq);
}

void virt_intc_irq_unmask(struct fm_pic_irq *pirq)
{
    fm_kprintf("%s irq = %d\n", __func__, pirq->irq);
}

void virt_intc_irq_eoi(struct fm_pic_irq *pirq)
{
    trig_irq = -1;
    fm_kprintf("%s irq = %d\n", __func__, pirq->irq);
}

fm_err_t virt_intc_irq_set_priority(struct fm_pic_irq *pirq, fm_uint32_t priority)
{
    fm_kprintf("%s irq = %d <%p>\n", __func__, pirq->irq, priority);
    return FM_EOK;
}

fm_err_t virt_intc_irq_set_triger_mode(struct fm_pic_irq *pirq, fm_uint32_t mode)
{
    fm_kprintf("%s irq = %d <%p>\n", __func__, pirq->irq, mode);
    return FM_EOK;
}

int virt_intc_irq_map(struct fm_pic *pic, int hwirq, fm_uint32_t mode)
{
    int irq = -1;

    irq = fm_pic_config_irq(pic, hwirq - 20, hwirq);

    if (irq >= 0)
    {
        fm_pic_irq_set_triger_mode(irq, mode);
    }

    return irq;
}

fm_err_t virt_intc_irq_parse(struct fm_pic *pic, struct fm_pic_cell_args *args, struct fm_pic_irq *out_pirq)
{
    fm_err_t err = FM_EOK;

    if (args->args_count == 2)
    {
        out_pirq->hwirq = args->args[0];
        out_pirq->mode = args->args[1] & FM_IRQ_MODE_MASK;
    }
    else
    {
        err = -FM_EINVAL;
    }

    return err;
}

struct fm_pic_ops virt_pic_ops =
{
    .name = "virt_pic",

    .irq_init = virt_intc_irq_init,
    .irq_finit = virt_intc_irq_finit,

    .irq_enable = virt_intc_irq_enable,
    .irq_disable = virt_intc_irq_disable,
    .irq_ack = virt_intc_irq_ack,
    .irq_mask = virt_intc_irq_mask,
    .irq_unmask = virt_intc_irq_unmask,
    .irq_eoi = virt_intc_irq_eoi,

    .irq_set_priority = virt_intc_irq_set_priority,
    .irq_set_triger_mode = virt_intc_irq_set_triger_mode,
    .irq_map = virt_intc_irq_map,
    .irq_parse = virt_intc_irq_parse,
};

static struct fm_pic virt_pic;

static fm_bool_t virt_intc_handler(void *data)
{
    fm_bool_t res = FM_FALSE;
    int hwirq;

    struct fm_pic_irq *pirq;

    hwirq = trig_irq;
    pirq = fm_pic_find_pirq(&virt_pic, hwirq);

    fm_pic_irq_ack(hwirq);

    fm_pic_handle_isr(pirq);

    fm_pic_irq_eoi(hwirq);

    res = FM_TRUE;

    return res;
}

static fm_err_t virt_intc_driver_probe(struct fm_platform_device *pdev)
{
    fm_err_t err = FM_EOK;
    LOG_I("pdev->num_resources = %d", pdev->num_resources);

    virt_pic.ops = &virt_pic_ops;
    fm_pic_default_name(&virt_pic);
    fm_pic_linear_irq(&virt_pic, pdev->num_resources);

    pic_args = fm_malloc(sizeof(struct fm_pic_cell_args) * pdev->num_resources);
    if (pic_args == FM_NULL)
    {
        return -FM_ENOMEM;
    }

    for (int i = 0; i < pdev->num_resources; i++)
    {
        pic_args[i].args_count = 2;
        pic_args[i].args[0] = pdev->resource[i].start;
        pic_args[i].args[1] = pdev->resource[i].flags;
        pic_args[i].data = &virt_pic;
        LOG_I("%d %d %p", pic_args[i].args_count, pic_args[i].args[0], pic_args[i].args[1]);

        fm_pic_map_irq(&pic_args[i]);
    }

    fm_pic_add_traps(virt_intc_handler, &virt_pic);

    return err;
}

static struct fm_platform_driver virt_intc_driver =
    {
        .name = "virt_device",

        .probe = virt_intc_driver_probe,
};

FM_PLATFORM_DRIVER_EXPORT(virt_intc_driver)

void virt_isr_handler(int vector, void *param)
{
    fm_kprintf("%s %d\n", __func__, vector);
}

int virt_intc_test(void)
{

    fm_pic_attach_irq(0, virt_isr_handler, &virt_pic, "isr_irq0", 0);
    fm_pic_attach_irq(1, virt_isr_handler, &virt_pic, "isr_irq1", 0);
    fm_pic_attach_irq(1, virt_isr_handler, &virt_pic, "isr_irq1", 0);
    fm_pic_attach_irq(2, virt_isr_handler, &virt_pic, "isr_irq2", 0);
    fm_pic_attach_irq(3, virt_isr_handler, &virt_pic, "isr_irq3", 0);

    return 0;
}

INIT_ENV_EXPORT(virt_intc_test);

#include <nr_micro_shell.h>

static void virt_intc(char argc, char *argv)
{
    if (!fm_strcmp("0", &argv[argv[1]]))
    {
        trig_irq = 0;
    }
    if (!fm_strcmp("1", &argv[argv[1]]))
    {
        trig_irq = 1;
    }
    if (!fm_strcmp("2", &argv[argv[1]]))
    {
        trig_irq = 2;
    }
    if (!fm_strcmp("3", &argv[argv[1]]))
    {
        trig_irq = 3;
    }

    fm_pic_do_traps();
}

NR_SHELL_CMD_EXPORT(virt_intc, virt_intc);
