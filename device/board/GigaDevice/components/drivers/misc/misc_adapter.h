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

#ifndef __MISC_ADAPTER_H__
#define __MISC_ADAPTER_H__

#include "gd32f4xx.h"
#include "gd32f4xx_misc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 生成MAC地址
 * 
 * @param mac_addr MAC地址指针
 */
void generate_mac_from_uid(uint8_t *mac_addr);

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
void get_build_time(uint16_t *year, uint8_t *month, uint8_t *date, uint8_t *hour, uint8_t *minute, uint8_t *second);

#ifdef __cplusplus
}
#endif

#endif /* __MISC_ADAPTER_H__ */
