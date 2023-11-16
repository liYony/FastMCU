#include "ethernetif.h"
#include "fm_mem.h"
#include "hc32_ll.h"
#include <dal.h>

#define PHY_HW_RST_DELAY (0x40U)

/* Global Ethernet handle*/
static stc_eth_handle_t EthHandle;

/* Ethernet Tx DMA Descriptor */
__ALIGN_BEGIN static stc_eth_dma_desc_t EthDmaTxDscrTab[ETH_TX_BUF_NUM];
/* Ethernet Rx DMA Descriptor */
__ALIGN_BEGIN static stc_eth_dma_desc_t EthDmaRxDscrTab[ETH_RX_BUF_NUM];
/* Ethernet Transmit Buffer */
__ALIGN_BEGIN static uint8_t EthTxBuff[ETH_TX_BUF_NUM][ETH_TX_BUF_SIZE];
/* Ethernet Receive Buffer */
__ALIGN_BEGIN static uint8_t EthRxBuff[ETH_RX_BUF_NUM][ETH_RX_BUF_SIZE];

/**
 * @brief  Initializes the Ethernet GPIO.
 * @param  None
 * @retval None
 */
static void Ethernet_GpioInit(void)
{

    /* Ethernet RMII pins configuration */
    /*
      ETH_SMI_MDIO ----------------> PA2
      ETH_SMI_MDC -----------------> PC1
      ETH_RMII_TX_EN --------------> PB11
      ETH_RMII_TXD0 ---------------> PB12(PE1)
      ETH_RMII_TXD1 ---------------> PB13(PE0)
      ETH_RMII_REF_CLK ------------> PA1
      ETH_RMII_CRS_DV -------------> PA7
      ETH_RMII_RXD0 ---------------> PC4
      ETH_RMII_RXD1 ---------------> PC5
    */
    GPIO_SetFunc(GPIO_PORT_A, GPIO_PIN_02, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_01, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_B, GPIO_PIN_11, GPIO_FUNC_11);
//    GPIO_SetFunc(GPIO_PORT_B, GPIO_PIN_12, GPIO_FUNC_11);
//    GPIO_SetFunc(GPIO_PORT_B, GPIO_PIN_13, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_E, GPIO_PIN_01, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_E, GPIO_PIN_00, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_A, GPIO_PIN_01, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_A, GPIO_PIN_07, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_04, GPIO_FUNC_11);
    GPIO_SetFunc(GPIO_PORT_C, GPIO_PIN_05, GPIO_FUNC_11);

    /* ETH_RST */
    /*
        RMII_nRST---------------->PB14(PE7)
    */
    stc_gpio_init_t stcGpioInit;
    GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    GPIO_Init(GPIO_PORT_E, GPIO_PIN_07, &stcGpioInit);

    /*RESET PHY*/
    GPIO_ResetPins(GPIO_PORT_E, GPIO_PIN_07);
    dal_delay_ms(PHY_HW_RST_DELAY);
    GPIO_SetPins(GPIO_PORT_E, GPIO_PIN_07);
}

void mcu_eth_init(struct netif *netif)
{
    stc_eth_init_t stcEthInit;

    /* Enable ETH clock */
    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_ETHMAC, ENABLE);

    /* Init Ethernet GPIO */
    Ethernet_GpioInit();

    uint16_t u16PhyReg = 0;
//    while (1)
//    {
//        ETH_PHY_ReadReg(&EthHandle, PHY_BCR, &u16PhyReg);
//        if (RESET == (u16PhyReg & PHY_SOFT_RESET))
//            break;
//        log_i("watting phy reset\r\n");
//        dal_delay_ms(100);
//    }

    /* Reset ETHERNET */
    (void)ETH_DeInit();
    /* Configure structure initialization */
    (void)ETH_CommStructInit(&EthHandle.stcCommInit);
    (void)ETH_StructInit(&stcEthInit);

    EthHandle.stcCommInit.u32Interface = ETH_MAC_IF_RMII;
    stcEthInit.stcMacInit.u32ReceiveAll = ETH_MAC_RX_ALL_ENABLE;

    while (1) // 等待网线连接后继续下一步
    {
        ETH_PHY_ReadReg(&EthHandle, PHY_BSR, &u16PhyReg);
        if (u16PhyReg & 0x0004)
        {
            break;
        }
        log_i("watting link up\r\n");
        dal_delay_ms(100);
    };

    log_i("init eth\r\n");
    int32_t _err_code = ETH_Init(&EthHandle, &stcEthInit);
    /* Configure ethernet peripheral */
    while (LL_OK != _err_code)
    {
        log_i("init eth error,code:%d\r\n", _err_code);
        dal_delay_ms(100);
        log_i("try reinit eth \r\n");
        _err_code = ETH_Init(&EthHandle, &stcEthInit);
    }
    log_i("init eth ok\r\n");

    /* Set netif link flag */
    netif->flags |= NETIF_FLAG_LINK_UP;

    /* Initialize TxRx Descriptors list: Chain Mode */
    (void)ETH_DMA_TxDescListInit(&EthHandle, EthDmaTxDscrTab, &EthTxBuff[0][0], ETH_TX_BUF_NUM);
    (void)ETH_DMA_RxDescListInit(&EthHandle, EthDmaRxDscrTab, &EthRxBuff[0][0], ETH_RX_BUF_NUM);

    /* set MAC */
    netif->hwaddr_len = 6U;
    netif->hwaddr[0] = (EthHandle.stcCommInit).au8MacAddr[0];
    netif->hwaddr[1] = (EthHandle.stcCommInit).au8MacAddr[1];
    netif->hwaddr[2] = (EthHandle.stcCommInit).au8MacAddr[2];
    netif->hwaddr[3] = (EthHandle.stcCommInit).au8MacAddr[3];
    netif->hwaddr[4] = (EthHandle.stcCommInit).au8MacAddr[4];
    netif->hwaddr[5] = (EthHandle.stcCommInit).au8MacAddr[5];
    /* maximum transfer unit */
    netif->mtu = 1500U;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    (void)ETH_Start();
}

struct pbuf *mcu_eth_rx(void)
{
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint32_t len;
    uint8_t *rxBuffer;
    __IO stc_eth_dma_desc_t *DmaRxDesc;
    uint32_t byteCnt;
    uint32_t bufferOffset;
    uint32_t payloadOffset;
    uint32_t i;

    /* Get received frame */
    if (LL_OK != ETH_DMA_GetReceiveFrame(&EthHandle))
    {
        return NULL;
    }

    /* Obtain the size of the packet */
    len = (EthHandle.stcRxFrame).u32Len;
    rxBuffer = (uint8_t *)(EthHandle.stcRxFrame).u32Buf;

    if (len > 0UL)
    {
        /* we allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, (uint16_t)len, PBUF_POOL);
    }
    if (p != NULL)
    {
        DmaRxDesc = (EthHandle.stcRxFrame).pstcFSDesc;
        bufferOffset = 0UL;
        for (q = p; q != NULL; q = q->next)
        {
            byteCnt = q->len;
            payloadOffset = 0UL;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size */
            while ((byteCnt + bufferOffset) > ETH_RX_BUF_SIZE)
            {
                /* Copy data to pbuf */
                (void)memcpy((uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), (uint8_t *)&(rxBuffer[bufferOffset]), (ETH_RX_BUF_SIZE - bufferOffset));
                /* Point to next descriptor */
                DmaRxDesc = (stc_eth_dma_desc_t *)(DmaRxDesc->u32Buf2NextDescAddr);
                rxBuffer = (uint8_t *)(DmaRxDesc->u32Buf1Addr);
                byteCnt = byteCnt - (ETH_RX_BUF_SIZE - bufferOffset);
                payloadOffset = payloadOffset + (ETH_RX_BUF_SIZE - bufferOffset);
                bufferOffset = 0UL;
            }

            /* Copy remaining data in pbuf */
            (void)memcpy((uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), (uint8_t *)&(rxBuffer[bufferOffset]), byteCnt);
            bufferOffset = bufferOffset + byteCnt;
        }
    }

    /* Release descriptors to DMA */
    DmaRxDesc = (EthHandle.stcRxFrame).pstcFSDesc;
    for (i = 0UL; i < (EthHandle.stcRxFrame).u32SegCount; i++)
    {
        DmaRxDesc->u32ControlStatus |= ETH_DMA_RXDESC_OWN;
        DmaRxDesc = (stc_eth_dma_desc_t *)(DmaRxDesc->u32Buf2NextDescAddr);
    }
    /* Clear Segment_Count */
    (EthHandle.stcRxFrame).u32SegCount = 0UL;

    /* When Rx Buffer unavailable flag is set, clear it and resume reception */
    if (RESET != ETH_DMA_GetStatus(ETH_DMA_FLAG_RUS))
    {
        /* Clear DMA RUS flag */
        ETH_DMA_ClearStatus(ETH_DMA_FLAG_RUS);
        /* Resume DMA reception */
        WRITE_REG32(CM_ETH->DMA_RXPOLLR, 0UL);
    }

    return p;
}

err_t mcu_eth_tx(struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;
    uint8_t *txBuffer;
    __IO stc_eth_dma_desc_t *DmaTxDesc;
    uint32_t byteCnt;
    uint32_t frameLength = 0UL;
    uint32_t bufferOffset;
    uint32_t payloadOffset;

    DmaTxDesc = EthHandle.stcTxDesc;
    txBuffer = (uint8_t *)((EthHandle.stcTxDesc)->u32Buf1Addr);
    bufferOffset = 0UL;

    /* Copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
    {
        /* If this buffer isn't available, goto error */
        if (0UL != (DmaTxDesc->u32ControlStatus & ETH_DMA_TXDESC_OWN))
        {
            errval = (err_t)ERR_USE;
            goto error;
        }

        /* Get bytes in LwIP buffer */
        byteCnt = q->len;
        payloadOffset = 0UL;
        /* Check if the length of data to copy is bigger than Tx buffer size */
        while ((byteCnt + bufferOffset) > ETH_TX_BUF_SIZE)
        {
            /* Copy data to Tx buffer*/
            (void)memcpy((uint8_t *)&(txBuffer[bufferOffset]), (uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), (ETH_TX_BUF_SIZE - bufferOffset));
            /* Point to next descriptor */
            DmaTxDesc = (stc_eth_dma_desc_t *)(DmaTxDesc->u32Buf2NextDescAddr);
            /* Check if the buffer is available */
            if (0UL != (DmaTxDesc->u32ControlStatus & ETH_DMA_TXDESC_OWN))
            {
                errval = (err_t)ERR_USE;
                goto error;
            }

            txBuffer = (uint8_t *)(DmaTxDesc->u32Buf1Addr);
            byteCnt = byteCnt - (ETH_TX_BUF_SIZE - bufferOffset);
            payloadOffset = payloadOffset + (ETH_TX_BUF_SIZE - bufferOffset);
            frameLength = frameLength + (ETH_TX_BUF_SIZE - bufferOffset);
            bufferOffset = 0UL;
        }
        /* Copy the remaining bytes */
        (void)memcpy((uint8_t *)&(txBuffer[bufferOffset]), (uint8_t *)&(((uint8_t *)q->payload)[payloadOffset]), byteCnt);
        bufferOffset = bufferOffset + byteCnt;
        frameLength = frameLength + byteCnt;
    }
    /* Prepare transmit descriptors to give to DMA */
    (void)ETH_DMA_SetTransFrame(&EthHandle, frameLength);
    errval = (err_t)ERR_OK;

error:
    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if (RESET != ETH_DMA_GetStatus(ETH_DMA_FLAG_UNS))
    {
        /* Clear DMA UNS flag */
        ETH_DMA_ClearStatus(ETH_DMA_FLAG_UNS);
        /* Resume DMA transmission */
        WRITE_REG32(CM_ETH->DMA_TXPOLLR, 0UL);
    }

    return errval;
}
