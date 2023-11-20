#ifndef __NETIF_ETHERNETIF_H__
#define __NETIF_ETHERNETIF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "lwip/err.h"
#include "lwip/netif.h"
#include <fm_type.h>
#include <dal.h>
#include <fmconfig.h>
#include <fm_timer.h>

#ifndef FM_LWIP_IPADDR
#define FM_LWIP_IPADDR "192.168.1.30"
#endif
#ifndef FM_LWIP_GWADDR
#define FM_LWIP_GWADDR "192.168.1.1"
#endif
#ifndef FM_LWIP_MSKADDR
#define FM_LWIP_MSKADDR "255.255.255.0"
#endif
#ifndef FM_LWIP_ETH_MTU
#define FM_LWIP_ETH_MTU (1500U)
#endif
#ifndef FM_LWIP_MAX_DHCP_TRIES
#define FM_LWIP_MAX_DHCP_TRIES (4U)
#endif
#ifndef FM_LWIP_ETH_HWADDR_LEN
#define FM_LWIP_ETH_HWADDR_LEN (6U)
#endif

#if LWIP_DHCP
typedef enum
{
    E_DHCP_START = 0,
    E_DHCP_CHECK,
    E_DHCP_SUCCESS,
    E_DHCP_FAILED
} dhcp_status_t;
#endif

void mcu_eth_init(struct netif *netif);
struct pbuf *mcu_eth_rx(void);
err_t mcu_eth_tx(struct pbuf *p);

err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);
u32_t sys_now(void);

#ifdef __cplusplus
}
#endif

#endif /* __NETIF_ETHERNETIF_H__ */
