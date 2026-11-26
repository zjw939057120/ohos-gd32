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

#ifndef _RTC_H
#define _RTC_H

#include "los_compiler.h"
#include "board.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// BCD转十进制
#define BCD2DEC(bcd)    ( (((bcd) >> 4) * 10) + ((bcd) & 0x0F) )
// 十进制(0~99)转BCD
#define DEC2BCD(dec)    ( (((dec) / 10) << 4) | ((dec) % 10) )

int RtcInit(void);
void rtc_setup(uint16_t year, uint8_t date, uint8_t month, uint8_t hour, uint8_t minute, uint8_t second);
void SetSysTime(void);
void parse_compile_time(uint16_t *year, uint8_t *month, uint8_t *day, 
                        uint8_t *hour, uint8_t *minute, uint8_t *second);
int SetRtcTime(uint16_t year, uint8_t month, uint8_t date,
                     uint8_t hour, uint8_t minute, uint8_t second);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* _RTC_H */
