# udp 通信DEMO

## 1 不指定远程IP

```c
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

static void udp_demo_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    ip_addr_t remote_ipaddr;
    unsigned char *temp = (unsigned char *)addr;
    IP4_ADDR(&remote_ipaddr, temp[0], temp[1], temp[2], temp[3]);
    log_d("recive from remote ip: %s", inet_ntoa(remote_ipaddr));
    udp_sendto(upcb, p, &remote_ipaddr, port);
    pbuf_free(p);
}

void udp_demo(void)
{
    struct udp_pcb *_udp_pcb = udp_new();
    if (_udp_pcb == NULL)
    {
        return;
    }
    else
    {
        if (udp_bind(_udp_pcb, IP_ADDR_ANY, 8080) == ERR_OK)
        {
            udp_recv(_udp_pcb, udp_demo_callback, NULL);
            printf("local_port %d\r\n", _udp_pcb->local_port);
        }
        else
        {
            return;
        }
    }
}

INITLV4_EXPORT(udp_demo);
```

## 2 指定远程IP

```c
#include "lwip/udp.h"
#include "lwip/ip_addr.h"

static void udp_demo_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    ip_addr_t remote_ipaddr;
    unsigned char *temp = (unsigned char *)addr;
    IP4_ADDR(&remote_ipaddr, temp[0], temp[1], temp[2], temp[3]);
    log_d("recive from remote ip: %s", inet_ntoa(remote_ipaddr));
    udp_sendto(upcb, p, &remote_ipaddr, port);
    pbuf_free(p);
}

void udp_demo(void)
{
    ip_addr_t remote_ipaddr;
    
    struct udp_pcb *_udp_pcb = udp_new();
    if (_udp_pcb == NULL)
    {
        return;
    }
    else
    {
        IP4_ADDR(&remote_ipaddr,192, 168, 1, 100);
        
        if(udp_connect(_udp_pcb, &remote_ipaddr, 8080) == ERR_OK)
        {
            udp_recv(_udp_pcb, udp_demo_callback, NULL);
            printf("local_port %d\r\n", _udp_pcb->local_port);
        }
        else
        {
            return;
        }
    }
}

INITLV4_EXPORT(udp_demo);
```
