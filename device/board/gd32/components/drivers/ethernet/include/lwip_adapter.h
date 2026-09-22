/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LWIP_ADAPTER_H
#define LWIP_ADAPTER_H

#include "gd32f4xx.h"
#include "stdint.h"
#include "lwipopts.h"

void lwip_stack_init(void);
#define DHCP_TIMEOUT_S 30
#define ETH_THREAD_PRIORITY 5
#define ENET_TASK_STACK_SIZE 2048
#define ENET_TASK_PRIORITY 30
#define DHCP_TASK_STACK_SIZE 2048
#define DHCP_TASK_PRIORITY 29

/* use DHCP */
#if LWIP_DHCP
#define USE_DHCP
#endif
/* use ENET interrupt */
#define USE_ENET_INTERRUPT

/* static IP address: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 2
#define IP_ADDR3 30

/* net mask */
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

/* gateway address */
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 2
#define GW_ADDR3 1

#define RMII_MODE // user have to provide the 50 MHz clock by soldering a 50 MHz oscillator

/* clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef MII_MODE
#define PHY_CLOCK_MCO
#endif

typedef union {
    UINT32 u32_addr;
    UINT8 u8_addr[4];
} IPUnion;

typedef struct {
    unsigned char useStaticIp;
    IPUnion ipaddr;
    IPUnion netmask;
    IPUnion gw;
    unsigned char useStaticMac;
    unsigned char macAddr[6];
} EthLinkInfo;

typedef enum {
    STATE_UPDATE_LINK_DOWN = 0,
    STATE_UPDATE_LINK_UP = 1,
} EthLinkState;

typedef void (*net_state_callBack)(EthLinkState state);

#define PHY_STATUS_CALLBACK(func, val)                                                                                 \
    {                                                                                                                  \
        do {                                                                                                           \
            if ((func) != NULL) {                                                                                      \
                (func)(val);                                                                                           \
            }                                                                                                          \
        } while (0);                                                                                                   \
    }

extern void enet_gpio_config(void);

 /**
 * @brief 获取网络链接状态
 * @return EthLinkState 网络链接状态
 */
EthLinkState get_network_link(void);

/**
 * @brief 更新静态IP地址
 * 
 * @param ip0 IP地址字节0
 * @param ip1 IP地址字节1
 * @param ip2 IP地址字节2
 * @param ip3 IP地址字节3
 * @param mask0 子网掩码字节0
 * @param mask1 子网掩码字节1
 * @param mask2 子网掩码字节2
 * @param mask3 子网掩码字节3
 * @param gw0 网关地址字节0
 * @param gw1 网关地址字节1
 * @param gw2 网关地址字节2
 * @param gw3 网关地址字节3
 */
void update_static_ip(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3,
                      uint8_t mask0, uint8_t mask1, uint8_t mask2, uint8_t mask3,
                      uint8_t gw0, uint8_t gw1, uint8_t gw2, uint8_t gw3);

#endif // end of #ifndef LWIP_ADAPTER_H