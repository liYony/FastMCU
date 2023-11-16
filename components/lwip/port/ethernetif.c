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

#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"
#include "lwip/timeouts.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'f'
#define IFNAME1 'm'

static struct netif lwip_netif;

dal_weak void mcu_eth_init(struct netif *netif)
{
    return;
}

dal_weak struct pbuf *mcu_eth_rx(void)
{
    return NULL;
}

dal_weak err_t mcu_eth_tx(struct pbuf *p)
{
    return ERR_IF;
}

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
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
    mcu_eth_init(netif);
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
    return mcu_eth_tx(p);
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
    return mcu_eth_rx();
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

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

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

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#if LWIP_DHCP

static dhcp_status_t dhcp_status = E_DHCP_START;

void lwip_dhcp_process(void)
{
    switch ((uint8_t)dhcp_status)
    {
    case E_DHCP_START:
    {
        dhcp_start(&lwip_netif);
        dhcp_status = E_DHCP_CHECK;
        log_i("check...........\r\n");
    }
    break;

    case E_DHCP_CHECK:
    {
        log_i("%d check...........\r\n", netif_dhcp_data(&lwip_netif)->tries);
        if (lwip_netif.ip_addr.addr != 0)
        {
            dhcp_status = E_DHCP_SUCCESS;
            log_i("====================================");
            log_i("dhcp ip address: %s", inet_ntoa(lwip_netif.ip_addr));
            log_i("dhcp gw address: %s", inet_ntoa(lwip_netif.gw));
            log_i("dhcp net mask  : %s", inet_ntoa(lwip_netif.netmask));
            log_i("====================================");
        }
        else if (netif_dhcp_data(&lwip_netif)->tries > FM_LWIP_MAX_DHCP_TRIES)
        {
            dhcp_status = E_DHCP_FAILED;
            lwip_netif.ip_addr.addr = inet_addr(FM_LWIP_IPADDR);
            lwip_netif.gw.addr = inet_addr(FM_LWIP_GWADDR);
            lwip_netif.netmask.addr = inet_addr(FM_LWIP_MSKADDR);
            log_i("====================================");
            log_i("ip address: %s", inet_ntoa(lwip_netif.ip_addr));
            log_i("gw address: %s", inet_ntoa(lwip_netif.gw));
            log_i("net mask  : %s", inet_ntoa(lwip_netif.netmask));
            log_i("====================================");
        }
    }
    break;

    default:
        break;
    }
}

void dhcp_fine_timer_callback(void *arg)
{
    dhcp_fine_tmr();
    if ((dhcp_status != E_DHCP_SUCCESS) && (dhcp_status != E_DHCP_FAILED))
    {
        lwip_dhcp_process();
    }
}

void dhcp_coarse_timer_callback(void *arg)
{
    dhcp_coarse_tmr();
}
#endif

err_t lwip_netif_init(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    lwip_init();

#if !LWIP_DHCP
    ipaddr.addr = inet_addr(FM_LWIP_IPADDR);
    gw.addr = inet_addr(FM_LWIP_GWADDR);
    netmask.addr = inet_addr(FM_LWIP_MSKADDR);
    log_i("====================================");
    log_i("ip address: %s", inet_ntoa(ipaddr));
    log_i("gw address: %s", inet_ntoa(gw));
    log_i("net mask  : %s", inet_ntoa(netmask));
    log_i("====================================");
#else
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&gw, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
#endif

    if (netif_add(&lwip_netif, (const ip_addr_t *)&ipaddr, (const ip_addr_t *)&netmask,
                  (const ip_addr_t *)&gw, NULL, &ethernetif_init, &ethernet_input) == NULL)
    {
        return ERR_IF;
    }
    else
    {
        netif_set_default(&lwip_netif);

        if (netif_is_link_up(&lwip_netif))
        {
            netif_set_up(&lwip_netif);
        }
        else
        {
            netif_set_down(&lwip_netif);
        }
    }

#if LWIP_DHCP
    dhcp_status = E_DHCP_START;
    dhcp_start(&lwip_netif);

    TIMER_CREATE_ATTR(dhcp_fine_timer);
    TIMER_CREATE_ATTR(dhcp_coarse_timer);
    fm_timer_create(dhcp_fine_timer_callback, TIMER_PERIODIC, NULL, &dhcp_fine_timer);
    fm_timer_create(dhcp_coarse_timer_callback, TIMER_PERIODIC, NULL, &dhcp_coarse_timer);
    fm_timer_start((fm_timer_id_t)&dhcp_fine_timer, DHCP_FINE_TIMER_MSECS);
    fm_timer_start((fm_timer_id_t)&dhcp_coarse_timer, DHCP_COARSE_TIMER_MSECS);
#endif
    return ERR_OK;
}

void l_init(void)
{
    lwip_netif_init();
}

INITLV3_EXPORT(l_init);

void l_poll(void)
{
    ethernetif_input(&lwip_netif);

    sys_check_timeouts();
}

POLLING_EXPORT(l_poll);
