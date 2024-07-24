#include <fmcu.h>
#include <devicetree.h>
#include <stdint.h>

#define DT_DRV_COMPAT st_stm32_uart

struct uart_device_config
{
    union
    {
        uint32_t port;
        uint8_t *base;
        uint32_t regs;
    };

    uint32_t sys_clk_freq;
};

struct stm32_pclken
{
    uint32_t bus;
    uint32_t enr;
};

/* device config */
struct uart_stm32_config
{
    const char *label;
    struct uart_device_config uconf;
    /* clock subsystem driving this peripheral */
    struct stm32_pclken pclken;
    /* initial hardware flow control, 1 for RTS/CTS */
    bool hw_flow_control;
    /* initial parity, 0 for none, 1 for odd, 2 for even */
    int parity;
};

static int uart_stm32_init(void *data)
{
    struct uart_stm32_config *cfg = (struct uart_stm32_config *)data;

    fm_kprintf("=================================================\n");
    fm_kprintf("label           : %s\n", cfg->label);
    fm_kprintf("base            : %p\n", cfg->uconf.base);
    fm_kprintf("sys_clk_freq    : %d\n", cfg->uconf.sys_clk_freq);
    fm_kprintf("bus             : %d\n", cfg->pclken.bus);
    fm_kprintf("enr             : %d\n", cfg->pclken.enr);
    fm_kprintf("hw_flow_control : %d\n", cfg->hw_flow_control);
    fm_kprintf("parity          : %d\n", cfg->parity);
    fm_kprintf("\n");

    return 0;
}

/************************************************************
usart1: serial@40011000 {
    compatible = "st,stm32-usart", "st,stm32-uart";
    reg = < 0x40011000 0x400 >;
    clocks = < &rcc 0x3 0x10 >;
    interrupts = < 0x25 0x0 >;
    status = "okay";
    label = "UART_1";
    pinctrl-0 = < &usart1_tx_pb6 &usart1_rx_pb7 >;
    current-speed = < 0x1c200 >;
};
usart2: serial@40004400 {
    compatible = "st,stm32-usart", "st,stm32-uart";
    reg = < 0x40004400 0x400 >;
    clocks = < &rcc 0x2 0x20000 >;
    interrupts = < 0x26 0x0 >;
    status = "okay";
    label = "UART_2";
    pinctrl-0 = < &usart2_tx_pa2 &usart2_rx_pa3 >;
    current-speed = < 0x1c200 >;
};
************************************************************/

#define STM32_UART_INIT(inst)                                                                      \
    static const struct uart_stm32_config uart_stm32_cfg_##inst = {                                \
        .label = DT_LABEL(DT_DRV_INST(inst)),                                                      \
        .uconf = {                                                                                 \
            .base = (uint8_t *)DT_INST_REG_ADDR(inst),                                             \
        },                                                                                         \
        .pclken = {.bus = DT_INST_CLOCKS_CELL(inst, bus), .enr = DT_INST_CLOCKS_CELL(inst, bits)}, \
        .hw_flow_control = DT_INST_PROP(inst, hw_flow_control),                                    \
        .parity = DT_ENUM_IDX_OR(DT_DRV_INST(inst), parity, 10),                                   \
    };                                                                                             \
    DT_INIT_BOARD_EXPORT(inst, uart_stm32_init, &uart_stm32_cfg_##inst);

DT_INST_FOREACH_STATUS_OKAY(STM32_UART_INIT)