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

#include "misc_adapter.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
 * @brief 获取构建日期时间
 * 
 * @param year 年指针
 * @param month 月指针
 * @param date 日指针
 * @param hour 小时指针
 * @param minute 分钟指针
 * @param second 秒指针
 */
void get_build_time(uint16_t *year, uint8_t *month, uint8_t *date, 
                    uint8_t *hour, uint8_t *minute, uint8_t *second) {
    // 1. 定义一个月份字符串映射表，用于将英文缩写转换为数字
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    
    char month_str[4] = {0};
    int temp_year, temp_date, temp_hour, temp_min, temp_sec;
    
    // 2. 解析 __DATE__ 宏 ("Mmm dd yyyy")
    // %3s 读取3个字符的月份缩写，%d 读取日期，%d 读取年份
    sscanf(__DATE__, "%3s %d %d", month_str, &temp_date, &temp_year);
    
    // 3. 遍历月份表，将缩写转换为 1~12 的数字
    *month = 0;
    for (int i = 0; i < 12; i++) {
        if (strncmp(month_str, months[i], 3) == 0) {
            *month = i + 1;
            break;
        }
    }
    
    // 4. 解析 __TIME__ 宏 ("hh:mm:ss")
    sscanf(__TIME__, "%d:%d:%d", &temp_hour, &temp_min, &temp_sec);
    
    // 5. 赋值给输出参数
    *year   = (uint16_t)temp_year;
    *date   = (uint8_t)temp_date;
    *hour   = (uint8_t)temp_hour;
    *minute = (uint8_t)temp_min;
    *second = (uint8_t)temp_sec;
}