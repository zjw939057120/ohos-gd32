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

#ifndef _RS485_ADAPTER_H
#define _RS485_ADAPTER_H

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

/*
 * @brief 初始化UART
 * @param None
 * @return None
 */
void init_rs485(void);

/*
 * @brief 注册UART中断
 * @param None
 * @return None
 */
void rs485_irq_register(void);

/*
 * @brief 发送数据
 * @param data 数据指针
 * @param len 数据长度
 * @return None
 */
void rs485_1_send(uint8_t *data, int len);
void rs485_2_send(uint8_t *data, int len);
void rs485_3_send(uint8_t *data, int len);

/*
 * @brief 接收数据
 * @param data 数据指针
 * @param len 数据长度
 * @return None
 */
void rs485_1_recv(uint8_t *data, int len);
void rs485_2_recv(uint8_t *data, int len);
void rs485_3_recv(uint8_t *data, int len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _RS485_ADAPTER_H */
