# LWIP 快速对接

## 1 需要实现的API
| **函数** | **描述**                      |
| ----------------- | ------------------- |
| mcu_eth_init()    | 网口初始化    |
| mcu_eth_rx()      | 网口接受      |
| mcu_eth_tx()      | 网口发送      |

### 1.1 网口初始化

```c
void mcu_eth_init(struct netif *netif)
```

- 复位PHY芯片
- 配置ETH外设
- 设置netif连接FLAG
- 设置MAC相关信息
- 设置MTU
- 设置其他FLAG
- 启动网卡

