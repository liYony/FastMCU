#include "lwip/opt.h"
#include "netif/etharp.h"  
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"

#include "ethernetif.h" 
#include "string.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'f'
#define IFNAME1 'm'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
    
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{  
    
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(struct netif *netif)
{
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* if no packet could be read, silently ignore this */
    if (p != NULL)
    {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }

} 

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    err_t errval;
    errval = ERR_OK;

    return errval;
}
#endif /* LWIP_ARP */

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    
    if (ethernetif == NULL)
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output = etharp_output;
    #else
    /* The user should write its own code in low_level_output_arp_off function */
    netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/**
 * @ingroup sys_time
 * Returns the current time in milliseconds,
 * may be the same as sys_jiffies or at least based on it.
 * Don't care for wraparound, this is only used for time diffs.
 * Not implementing this function means you cannot use some modules (e.g. TCP
 * timestamps, internal timeouts for NO_SYS==1).
 */
u32_t sys_now(void)
{
    return (dal_get_systick());
}

#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"

#define FM_LWIP_IPADDR  "192.168.1.30"
#define FM_LWIP_GWADDR  "192.168.1.1"
#define FM_LWIP_MSKADDR "255.255.255.0"

struct netif lwip_netif;            /* 定义一个全局的网络接口 */

uint8_t lwip_comm_init(void)
{
//    uint8_t retry = 0;
    ip_addr_t ipaddr;                           /* ip地址 */
    ip_addr_t netmask;                          /* 子网掩码 */
    ip_addr_t gw;                               /* 默认网关 */

//    if (ethernet_mem_malloc())return 1;         /* 内存申请失败*/

//    lwip_comm_default_ip_set(&lwipdev);         /* 设置默认IP等信息 */

//    while (ethernet_init())                     /* 初始化以太网芯片,如果失败的话就重试5次 */
//    {
//        retry++;

//        if (retry > 5)
//        {
//            retry = 0;                          /* 以太网芯片初始化失败 */
//            return 3;
//        }
//    }

    lwip_init();                                /* 初始化LWIP内核 */
    
#if !LWIP_DHCP
        ipaddr.addr = inet_addr(FM_LWIP_IPADDR);
        gw.addr = inet_addr(FM_LWIP_GWADDR);
        netmask.addr = inet_addr(FM_LWIP_MSKADDR);
        printf("\n");
        printf("ip address: %s\n", inet_ntoa(ipaddr));
        printf("gw address: %s\n", inet_ntoa(gw));
        printf("net mask  : %s\n", inet_ntoa(netmask));
#else
        IP4_ADDR(&ipaddr, 0, 0, 0, 0);
        IP4_ADDR(&gw, 0, 0, 0, 0);
        IP4_ADDR(&netmask, 0, 0, 0, 0);
#endif

    if (netif_add(&lwip_netif, (const ip_addr_t *)&ipaddr, (const ip_addr_t *)&netmask, 
        (const ip_addr_t *)&gw, NULL, &ethernetif_init, &ethernet_input) == NULL)
    {
        return 4;                           /* 网卡添加失败 */
    }
    else                                    /* 网口添加成功后,设置netif为默认值,并且打开netif网口 */
    {
        netif_set_default(&lwip_netif);     /* 设置netif为默认网口 */

        if (netif_is_link_up(&lwip_netif))
        {
            netif_set_up(&lwip_netif);      /* 打开netif网口 */
        }
        else
        {
            netif_set_down(&lwip_netif);
        }
    }

#if LWIP_DHCP                               /* 如果使用DHCP的话 */
    dhcp_start(&lwip_netif);                /* 开启DHCP服务 */
#endif
    return 0;                               /* 操作OK. */
}

