/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

/*
 * Debug console UART driver for GD32F470ZG.
 * USART0 on PA9 (TX) / PA10 (RX), AF7, 115200 8N1.
 * Output goes through UartPutc (used by libc/dprintf.c printf); shell input
 * arrives via the USART0 RBNE interrupt which signals g_shellInputEvent.
 */

#include "misc_adapter.h"
#include "los_interrupt.h"
#include "los_event.h"
#include "pin_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// GD32F470 唯一设备ID的基地址
#define GD32_UNIQUE_ID_BASE 0x1FFF7A10

/**
 * @brief 基于GD32芯片唯一ID生成MAC地址
 * @param mac_addr 用于存储生成的48位MAC地址的数组指针
 */
void generate_mac_from_uid(uint8_t *mac_addr)
{
    // 1. 获取指向96位(12字节)唯一ID的指针
    uint8_t *uid_ptr = (uint8_t *)GD32_UNIQUE_ID_BASE;
    
    // 2. 异或折叠法：将12字节的ID分为前6字节和后6字节，进行按位异或运算
    for (uint8_t i = 0; i < 6; i++) {
        mac_addr[i] = uid_ptr[i] ^ uid_ptr[i + 6];
    }
    
    // 3. 设置本地管理地址标志位（关键步骤）
    // 对数组的第一个字节 (mac_addr[0]) 进行位运算，而不是对指针本身
    mac_addr[0] |= 0x02;  // 将第1个字节的次低位置1，表明这是本地生成的地址
    mac_addr[0] &= 0xFE;  // 确保第一个字节的最低位为0，表明这是一个单播地址
}

/**
 * @brief 解析编译时间
 * 
 * @param year 年指针
 * @param month 月指针
 * @param date 日指针
 * @param hour 小时指针
 * @param minute 分钟指针
 * @param second 秒指针
 */
void parse_compile_time(uint16_t *year, uint8_t *month, uint8_t *day, 
                        uint8_t *hour, uint8_t *minute, uint8_t *second) {
    const char *date_str = __DATE__; // 格式: "Mmm dd yyyy"
    const char *time_str = __TIME__; // 格式: "hh:mm:ss"

    // 1. 解析年份 (取字符串第 7~10 位)
    *year = (date_str[7] - '0') * 1000 + (date_str[8] - '0') * 100 + 
            (date_str[9] - '0') * 10 + (date_str[10] - '0');

    // 2. 解析月份 (通过比对前 3 个字符的缩写)
    const char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    *month = 1; // 默认值
    for (int i = 0; i < 12; i++) {
        if (strncmp(date_str, months[i], 3) == 0) {
            *month = i + 1;
            break;
        }
    }

    // 3. 解析日期 (取字符串第 4~5 位，注意处理前导空格)
    // 如果第 4 位是空格，说明是个位数日期；否则按两位数计算
    if (date_str[4] == ' ') {
        *day = date_str[5] - '0';
    } else {
        *day = (date_str[4] - '0') * 10 + (date_str[5] - '0');
    }

    // 4. 解析时、分、秒 (利用固定格式 "hh:mm:ss" 直接通过下标计算)
    *hour   = (time_str[0] - '0') * 10 + (time_str[1] - '0');
    *minute = (time_str[3] - '0') * 10 + (time_str[4] - '0');
    *second = (time_str[6] - '0') * 10 + (time_str[7] - '0');
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
