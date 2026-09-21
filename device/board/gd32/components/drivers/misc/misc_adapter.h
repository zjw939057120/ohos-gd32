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

#ifndef _MISC_ADAPTER_H
#define _MISC_ADAPTER_H

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "los_event.h"
#include "los_compiler.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief 生成MAC地址
 * 
 * @param mac_addr MAC地址指针
 */
void generate_mac_from_uid(uint8_t *mac_addr);

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
                        uint8_t *hour, uint8_t *minute, uint8_t *second);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _MISC_ADAPTER_H */
