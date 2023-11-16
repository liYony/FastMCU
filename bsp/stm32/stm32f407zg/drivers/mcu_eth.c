#include <dal.h>
#include "stm32f4xx_hal.h"
#include "ethernetif.h"

ETH_HandleTypeDef heth;

/* Ethernet Tx DMA Descriptor */
__ALIGN_BEGIN static ETH_DMADescTypeDef DMATxDscrTab[ETH_TXBUFNB];
/* Ethernet Rx DMA Descriptor */
__ALIGN_BEGIN static ETH_DMADescTypeDef DMARxDscrTab[ETH_RXBUFNB];
/* Ethernet Transmit Buffer */
__ALIGN_BEGIN static uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];
/* Ethernet Receive Buffer */
__ALIGN_BEGIN static uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];

/**
 * @brief ETH MSP Initialization
 * This function configures the hardware resources used in this example
 * @param heth: ETH handle pointer
 * @retval None
 */
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (heth->Instance == ETH)
    {
        /* USER CODE BEGIN ETH_MspInit 0 */

        /* USER CODE END ETH_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_ETH_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        /**ETH GPIO Configuration
        PC1     ------> ETH_MDC
        PA1     ------> ETH_REF_CLK
        PA2     ------> ETH_MDIO
        PA7     ------> ETH_CRS_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PG11     ------> ETH_TX_EN
        PG13     ------> ETH_TXD0
        PG14     ------> ETH_TXD1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        /* USER CODE BEGIN ETH_MspInit 1 */

        /* USER CODE END ETH_MspInit 1 */
    }
}

/**
 * @brief ETH MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param heth: ETH handle pointer
 * @retval None
 */
void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
    if (heth->Instance == ETH)
    {
        /* USER CODE BEGIN ETH_MspDeInit 0 */

        /* USER CODE END ETH_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ETH_CLK_DISABLE();

        /**ETH GPIO Configuration
        PC1     ------> ETH_MDC
        PA1     ------> ETH_REF_CLK
        PA2     ------> ETH_MDIO
        PA7     ------> ETH_CRS_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PG11     ------> ETH_TX_EN
        PG13     ------> ETH_TXD0
        PG14     ------> ETH_TXD1
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7);

        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14);

        /* USER CODE BEGIN ETH_MspDeInit 1 */

        /* USER CODE END ETH_MspDeInit 1 */
    }
}

static void Eth_Reset(void)
{
    /* PHY RESET: PD3 */
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    dal_delay_ms(50);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    dal_delay_ms(50);
}

HAL_StatusTypeDef Bsp_Eth_Init(void)
{
    HAL_StatusTypeDef ret;

    uint8_t MACAddr[6];

    HAL_ETH_DeInit(&heth);

    Eth_Reset();

    ETH->DMABMR |= ETH_DMABMR_SR;

    /* Init ETH */
    MACAddr[0] = 0x02;
    MACAddr[1] = 0x00;
    MACAddr[2] = 0x00;
    MACAddr[3] = 0x00;
    MACAddr[4] = 0x00;
    MACAddr[5] = 0x00;
    heth.Instance = ETH;
    heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
    heth.Init.PhyAddress = ETHERNET_PHY_ADDRESS;
    heth.Init.MACAddr = &MACAddr[0];
    heth.Init.RxMode = ETH_RXINTERRUPT_MODE; // rx mode
    heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
    heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
    heth.Init.Speed = ETH_SPEED_100M; // speed
    heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;

    /* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
    ret = HAL_ETH_Init(&heth);
    if (ret == HAL_OK)
        log_d("eth hardware init sucess...\n");
    else
        log_d("eth hardware init faild...\n");

    /* Initialize Tx Descriptors list: Chain Mode */
    HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);

    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
    /* Enable MAC and DMA transmission and reception */

    return ret;
}

void mcu_eth_init(struct netif *netif)
{
    HAL_StatusTypeDef hal_eth_init_status;

    hal_eth_init_status = Bsp_Eth_Init();

    if (hal_eth_init_status == HAL_OK)
    {
        /* Set netif link flag */
        netif->flags |= NETIF_FLAG_LINK_UP;
    }

#if LWIP_ARP || LWIP_ETHERNET

    /* set MAC hardware address length */
    netif->hwaddr_len = FM_LWIP_ETH_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = heth.Init.MACAddr[0];
    netif->hwaddr[1] = heth.Init.MACAddr[1];
    netif->hwaddr[2] = heth.Init.MACAddr[2];
    netif->hwaddr[3] = heth.Init.MACAddr[3];
    netif->hwaddr[4] = heth.Init.MACAddr[4];
    netif->hwaddr[5] = heth.Init.MACAddr[5];

    /* maximum transfer unit */
    netif->mtu = FM_LWIP_ETH_MTU;

#if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#else
    netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */

#endif /* LWIP_ARP || LWIP_ETHERNET */

    HAL_ETH_Start(&heth);
}

struct pbuf *mcu_eth_rx(void)
{
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    uint16_t len = 0;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t i = 0;

    /* get received frame */
    if (HAL_ETH_GetReceivedFrame(&heth) != HAL_OK)
    {
        //         log_e("receive frame faild\n");
        return NULL;
    }
    /*Obtain the size of the packet and put it into the "len" variable.*/
    len = heth.RxFrameInfos.length;
    buffer = (uint8_t *)heth.RxFrameInfos.buffer;

    //     log_i("receive frame len : %d\n", len);

    if (len > 0)
    {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }

    if (p != NULL)
    {
        dmarxdesc = heth.RxFrameInfos.FSRxDesc;
        bufferoffset = 0;
        for (q = p; q != NULL; q = q->next)
        {
            byteslefttocopy = q->len;
            payloadoffset = 0;

            while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE)
            {
                /* Copy data to pbuf */
                memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset),
                       (uint8_t *)((uint8_t *)buffer + bufferoffset),
                       (ETH_RX_BUF_SIZE - bufferoffset));

                /* Point to next descriptor */
                dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            /* Copy remaining data in pbuf */
            memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset),
                   (uint8_t *)((uint8_t *)buffer + bufferoffset), byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
    }

    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dmarxdesc = heth.RxFrameInfos.FSRxDesc;
    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (i = 0; i < heth.RxFrameInfos.SegCount; i++)
    {
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }

    /* Clear Segment_Count */
    heth.RxFrameInfos.SegCount = 0;

    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if ((heth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
    {
        /* Clear RBUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        heth.Instance->DMARPDR = 0;
    }
    return p;
}

err_t mcu_eth_tx(struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;

    uint8_t *buffer = (uint8_t *)(heth.TxDesc->Buffer1Addr);

    __IO ETH_DMADescTypeDef *DmaTxDesc;

    uint32_t bufferoffset = 0;
    uint32_t framelength = 0;

    uint32_t byteslefttocopy = 0;

    uint32_t payloadoffset = 0;

    DmaTxDesc = heth.TxDesc;
    bufferoffset = 0;

    if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
    {
        errval = ERR_USE;
        goto error;
    }

    /* copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
    {
        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len;
        payloadoffset = 0;

        /*Check if the length of data to copy is bigger than Tx buffer size*/
        while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE)
        {
            /* Copy data to Tx buffer*/
            memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset),
                   (uint8_t *)((uint8_t *)q->payload + payloadoffset),
                   (ETH_TX_BUF_SIZE - bufferoffset));

            /* Point to next descriptor */
            DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);

            /* Check if the buffer is available */
            if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
            {
                errval = ERR_USE;
                goto error;
            }

            buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);

            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }

        /* Copy the remaining bytes */
        memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset),
               (uint8_t *)((uint8_t *)q->payload + payloadoffset), byteslefttocopy);
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }

    /* Prepare transmit descriptors to give to DMA */
    HAL_ETH_TransmitFrame(&heth, framelength);

    errval = ERR_OK;

error:

    if ((heth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
    {
        /* Clear TUS ETHERNET DMA flag */
        heth.Instance->DMASR = ETH_DMASR_TUS;

        /* Resume DMA transmission*/
        heth.Instance->DMATPDR = 0;
    }

    return errval;
}
