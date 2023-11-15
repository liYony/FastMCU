#ifndef __NETIF_ETHERNETIF_H__
#define __NETIF_ETHERNETIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/err.h"
#include "lwip/netif.h"
#include <fm_type.h>
#include <dal.h>

#define NIOCTL_GADDR        0x01
#ifndef RT_LWIP_ETH_MTU
#define ETHERNET_MTU        1500
#else
//#define ETHERNET_MTU        RT_LWIP_ETH_MTU
#endif

/* eth flag with auto_linkup or phy_linkup */
#define ETHIF_LINK_AUTOUP   0x0000
#define ETHIF_LINK_PHYUP    0x0100

struct eth_device
{
    /* network interface for lwip */
    struct netif *netif;

    uint16_t flags;
    uint8_t  link_changed;
    uint8_t  link_status;
    uint8_t  rx_notice;

    /* eth device interface */
    struct pbuf* (*eth_rx)(struct eth_device dev);
    int (*eth_tx)(struct eth_device dev, struct pbuf* p);
};

err_t ethernetif_init(struct netif *netif);  /* 网卡初始化函数 */
void ethernetif_input(struct netif *netif);  /* 数据包输入函数 */
u32_t sys_now(void);

#ifdef __cplusplus
}
#endif

#endif /* __NETIF_ETHERNETIF_H__ */
