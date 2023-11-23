#include <dal.h>
#include <board_config.h>
#include "hc32_ll.h"

typedef struct 
{
    CM_DMA_TypeDef *unit_dma;
    uint32_t ch;
    uint32_t clk;
    uint32_t target;
    en_event_src_t e_src;
} spi_dma_cfg_t;

typedef struct
{
    dal_spi_number_t spi;
    dal_gpio_instance_t cs;
    CM_SPI_TypeDef *unit_spi;
    uint32_t fcg_clk;
    spi_dma_cfg_t *dma_tx;
    spi_dma_cfg_t *dma_rx;
} spi_info_t;

static spi_dma_cfg_t spi4_txdma_cfg = {CM_DMA1, DMA_CH0, FCG0_PERIPH_DMA1, AOS_DMA1_0, EVT_SRC_SPI4_SPTI};
static spi_dma_cfg_t spi4_rxdma_cfg = {CM_DMA1, DMA_CH1, FCG0_PERIPH_DMA1, AOS_DMA1_1, EVT_SRC_SPI4_SPRI};

static spi_info_t spi_info[] =
{
    {DAL_SPI_1, {DAL_GPIOE, DAL_PIN5}, CM_SPI4, FCG1_PERIPH_SPI4, &spi4_txdma_cfg, &spi4_rxdma_cfg},
};

static int hc32_spi_dma_init(spi_info_t *info, spi_dma_cfg_t *dma)
{
    stc_dma_init_t stcDmaInit;

    if (dma == NULL)
    {
        return -1;
    }

    FCG_Fcg0PeriphClockCmd(dma->clk | FCG0_PERIPH_AOS, ENABLE);
    AOS_SetTriggerEventSrc(dma->target, dma->e_src);

    /* Config Dma */
    DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = 0;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;

    if (dma == info->dma_tx)
    {
        stcDmaInit.u32DestAddr = (uint32_t)(&info->unit_spi->DR);
        stcDmaInit.u32SrcAddr = 0;
        stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
        stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    }
    else
    {
        stcDmaInit.u32DestAddr = 0;
        stcDmaInit.u32SrcAddr = (uint32_t)(&info->unit_spi->DR);
        stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
        stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    }

    /* Init Dma */
    if (LL_OK != DMA_Init(dma->unit_dma, dma->ch, &stcDmaInit))
    {
        return -1;
    }

    /* Enable Dma */
    DMA_Cmd(dma->unit_dma, ENABLE);

    return 0;
}

static int hc32_spi_init(spi_info_t *info, uint16_t clk_div)
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
    stcSpiInit.u32SpiMode = SPI_MD_0;
    stcSpiInit.u32BaudRatePrescaler = clk_div;
    stcSpiInit.u32DataBits = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit = SPI_FIRST_MSB;
    stcSpiInit.u32FrameLevel = SPI_1_FRAME;

    if (LL_OK != SPI_Init(info->unit_spi, &stcSpiInit))
    {
        return -1;
    }

    return 0;
}

static int hc32_spi_dma_trans(spi_info_t *info, const uint8_t *tx_buf, void *rx_buf, uint32_t length)
{
    if (info == NULL)
    {
        return -1;
    }

    SPI_Cmd(info->unit_spi, DISABLE);
    if (NULL != tx_buf)
    {
        DMA_SetSrcAddr(info->dma_tx->unit_dma, info->dma_tx->ch, (uint32_t)tx_buf);
        DMA_SetTransCount(info->dma_tx->unit_dma, info->dma_tx->ch, length);
        DMA_ChCmd(info->dma_tx->unit_dma, info->dma_tx->ch, ENABLE);
    }
    if (NULL != rx_buf)
    {
        DMA_SetDestAddr(info->dma_rx->unit_dma, info->dma_rx->ch, (uint32_t)rx_buf);
        DMA_SetTransCount(info->dma_rx->unit_dma, info->dma_rx->ch, length);
        DMA_ChCmd(info->dma_rx->unit_dma, info->dma_rx->ch, ENABLE);
    }
    SPI_Cmd(info->unit_spi, ENABLE);
    return LL_OK;
}

int mcu_spi_init(dal_spi_number_t spi, uint32_t freq)
{
    if (spi >= (sizeof(spi_info) / sizeof(spi_info[0])))
    {
        return -1;
    }
    dal_gpio_config(spi_info[spi].cs.port, spi_info[spi].cs.pin, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);
    hc32_spi_init(&spi_info[spi], SPI_BR_CLK_DIV8); // 120 / 8 = 15
    hc32_spi_dma_init(&spi_info[spi], spi_info[spi].dma_tx);
    hc32_spi_dma_init(&spi_info[spi], spi_info[spi].dma_rx);

    return 0;
}

int mcu_spi_xfer(dal_spi_number_t spi, dal_spi_message_t *msg)
{
    if (spi >= (sizeof(spi_info) / sizeof(spi_info[0])))
    {
        return -1;
    }

    if (msg->cs_take)
    {
        dal_gpio_write(spi_info[spi].cs.port, spi_info[spi].cs.pin, DAL_LVL_LOW);
    }

    hc32_spi_dma_trans(&spi_info[spi], msg->send_buf, msg->recv_buf, msg->length);

    /* Wait for the spi transfer complete */
    while (RESET != SPI_GetStatus(spi_info[spi].unit_spi, SPI_FLAG_IDLE));
    /* clear error flag */
    SPI_ClearStatus(spi_info[spi].unit_spi, SPI_FLAG_CLR_ALL | SPI_FLAG_RX_BUF_FULL);

    if (msg->cs_release)
    {
        dal_gpio_write(spi_info[spi].cs.port, spi_info[spi].cs.pin, DAL_LVL_HIGH);
    }

    return 0;
}

int mcu_spi_attach_cs(dal_spi_number_t spi, dal_gpio_instance_t cs)
{
    if (spi >= (sizeof(spi_info) / sizeof(spi_info[0])))
    {
        return -1;
    }

    spi_info[spi].cs.port = cs.port;
    spi_info[spi].cs.pin = cs.pin;

    dal_gpio_config(spi_info[spi].cs.port, spi_info[spi].cs.pin, DAL_GPIO_OUTPUT, DAL_GPIO_NOPULL);

    return 0;
}
