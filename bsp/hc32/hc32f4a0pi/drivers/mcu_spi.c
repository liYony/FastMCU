#include <dal.h>
#include "hc32_ll.h"
#include <board_config.h>

typedef struct
{
    dal_spi_number_t spi;
    CM_SPI_TypeDef *unit_spi;
    uint32_t fcg_clk;
    
} spi_info_t;

spi_info_t spi_info[] = 
{
    {DAL_SPI_1, CM_SPI4, FCG1_PERIPH_SPI4}
};

//static void hc32_spi_dma_init()
//{
//    
//}

static void hc32_spi_init(spi_info_t *info, uint16_t clk_div)
{
    stc_spi_init_t stcSpiInit;

    /* Configure Port */
    hc32_borad_spi_init(info->unit_spi);

    /* Configuration SPI */
    FCG_Fcg1PeriphClockCmd(info->fcg_clk, ENABLE);
    SPI_StructInit(&stcSpiInit);
    stcSpiInit.u32WireMode = SPI_3_WIRE;
    stcSpiInit.u32TransMode = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave = SPI_MASTER;
    stcSpiInit.u32Parity = SPI_PARITY_INVD;
    stcSpiInit.u32SpiMode = SPI_MD_1;
    stcSpiInit.u32BaudRatePrescaler = clk_div;
    stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
    stcSpiInit.u32FrameLevel = SPI_1_FRAME;
    
    (void)SPI_Init(info->unit_spi, &stcSpiInit);
}

int mcu_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    return 0;
}

int mcu_spi_xfer(dal_spi_number_t spi, dal_spi_message_t *msg)
{
    return 0;
}

#include <qk_section.h>

void spi_test(void)
{
    hc32_spi_init(&spi_info[0], SPI_BR_CLK_DIV4);
}

INITLV4_EXPORT(spi_test);
