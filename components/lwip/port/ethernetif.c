#include <arch/cc.h>
#include <dal.h>
#include <ethernetif.h>
#include "netif/etharp.h"

u32_t sys_now(void)
{
    return (dal_get_systick());
}

/* Network interface name */
#define IFNAME0 'f'
#define IFNAME1 'm'

static void low_level_init(struct netif *netif)
{
    
}
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    
}
/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    
}

/**
 * @brief  This function should be called when a packet is ready to be read from the interface.
 * @param  netif                        The network interface structure for this ethernetif.
 * @retval None
 */

void ethernetif_input(struct netif *netif)
{
    err_t err;
    struct pbuf *p;
    /* Move received packet into a new pbuf */
    p = low_level_input(netif);
    /* No packet could be read, silently ignore this */
    if(p == NULL)
    {
        return;
    }
    /* Entry point to the LwIP stack */
    err = netif->input(p, netif);
    if(err != (err_t)ERR_OK)
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        (void)pbuf_free(p);
        p = NULL;
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
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    #if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
    #endif /* LWIP_NETIF_HOSTNAME */

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

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

#if LWIP_NETIF_LINK_CALLBACK
/**
 * @brief  Link callback function, this function is called on change of link status
 *         to update low level driver configuration.
 * @param  netif: The network interface
 * @retval None
 */
void ethernetif_link_config(struct netif *netif)
{
    __IO uint32_t tickstart = 0;
    uint32_t regvalue = 0;

    if (netif_is_link_up(netif))
    {
    }
    else
    {
        /* Stop MAC interface */
        HAL_ETH_Stop(&heth);
    }
}
#endif /* LWIP_NETIF_LINK_CALLBACK */
