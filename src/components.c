#include <fmcu.h>

#ifdef FM_USING_COMPONENTS_INIT
/*
 * Components Initialization will initialize some driver and components as following
 * order:
 * fmi_start         --> 0
 * BOARD_EXPORT      --> 1
 * fmi_board_end     --> 1.end
 *
 * DEVICE_EXPORT     --> 2
 * COMPONENT_EXPORT  --> 3
 * FS_EXPORT         --> 4
 * ENV_EXPORT        --> 5
 * APP_EXPORT        --> 6
 *
 * fmi_end           --> 6.end
 *
 * These automatically initialization, the driver or component initial function must
 * be defined with:
 * INIT_BOARD_EXPORT(fn);
 * INIT_DEVICE_EXPORT(fn);
 * ...
 * INIT_APP_EXPORT(fn);
 * etc.
 */
static int fmi_start(void)
{
    return 0;
}
INIT_EXPORT(fmi_start, "0");

static int fmi_board_start(void)
{
    return 0;
}
INIT_EXPORT(fmi_board_start, "0.end");

static int fmi_board_end(void)
{
    return 0;
}
INIT_EXPORT(fmi_board_end, "1.end");

static int fmi_end(void)
{
    return 0;
}
INIT_EXPORT(fmi_end, "6.end");

/**
 * @brief  Onboard components initialization. In this function, the board-level
 *         initialization function will be called to complete the initialization
 *         of the on-board peripherals.
 */
void fm_components_board_init(void)
{
#ifdef FM_DEBUGING_INIT
    int result;
    const struct fm_init_desc *desc;
    
    fm_kprintf("do components board initialization.\r\n");
    for (desc = &__fm_init_desc_fmi_board_start; desc < &__fm_init_desc_fmi_board_end; desc ++)
    {
        fm_kprintf("initialize %s", desc->fn_name);
        result = desc->fn();
        fm_kprintf(":%d done\r\n", result);
    }
#else
    volatile const init_fn_t *fn_ptr;

    for (fn_ptr = &__fm_init_fmi_board_start; fn_ptr < &__fm_init_fmi_board_end; fn_ptr++)
    {
        (*fn_ptr)();
    }
#endif /* FM_DEBUGING_INIT */
}

/**
 * @brief  FastMCU Components Initialization.
 */
void fm_components_init(void)
{
#ifdef FM_DEBUGING_INIT
    int result;
    const struct fm_init_desc *desc;

    fm_kprintf("do components initialization.\r\n");
    for (desc = &__fm_init_desc_fmi_board_end; desc < &__fm_init_desc_fmi_end; desc ++)
    {
        fm_kprintf("initialize %s", desc->fn_name);
        result = desc->fn();
        fm_kprintf(":%d done\r\n", result);
    }
#else
    volatile const init_fn_t *fn_ptr;

    for (fn_ptr = &__fm_init_fmi_board_end; fn_ptr < &__fm_init_fmi_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }
#endif /* FM_DEBUGING_INIT */
}
#endif /* FM_USING_COMPONENTS_INIT */

void fm_hw_board_init(void);

void fmcu_startup(void)
{
    /* board level initialization */
    fm_hw_board_init();

    /* update delay param */
    fm_update_delay_param();

    /* show FastMCU version */
    fm_show_version();

#ifdef FM_USING_COMPONENTS_INIT
    /* FastMCU components initialization */
    fm_components_init();
#endif
}
