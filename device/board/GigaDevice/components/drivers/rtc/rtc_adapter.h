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

#ifndef __RTC_ADAPTER_H__
#define __RTC_ADAPTER_H__

#include "gd32f4xx.h"
#include "gd32f4xx_rtc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BCD码转换为十进制
 * 
 * @param val BCD码值
 * @return uint8_t 十进制值
 */
#define BCD2DEC(val) 	(((val) >> 4) * 10 + ((val)&0x0F))

/**
 * @brief 十进制转换为BCD码
 * 
 * @param val 十进制值
 * @return uint8_t BCD码值
 */
#define DEC2BCD(val) 	(((val) / 10) << 4 | ((val) % 10))

/**
 * @brief 初始化RTC外设
 * 
 */
void init_rtc(void);

/**
 * @brief 设置RTC时间
 * 
 * @param year 年份
 * @param month 月份
 * @param date 日
 * @param hour 小时
 * @param minute 分钟
 * @param second 秒
 * @return int 0 成功 -1 失败
 */
int rtc_set_datetime(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_ADAPTER_H__ */
