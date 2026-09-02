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

#ifndef __QUEUE_ADAPTER_H__
#define __QUEUE_ADAPTER_H__

#include <stdio.h>
#include "gd32f4xx.h"
#include "gd32f4xx_usart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_QUEUE_LEN 10  /* 最大队列长度 */
#define MAX_MSG_SIZE 255  /* 最大消息大小 */
/**
 * @brief 初始化队列
 * 
 */
void queue_init(void);
/**
 * @brief 写入RS485_1队列
 * 
 * @param bufferAddr 数据指针
 * @param bufferSize 数据大小
 * @return UINT32 0 成功
 */
UINT32 rs485_1_write(VOID *bufferAddr,UINT32 bufferSize);
/**
 * @brief 读取RS485_1队列数据
 * 
 * @param bufferAddr 数据指针
 * @param bufferSize 数据大小
 * @return UINT32 0 成功
 */
UINT32 rs485_1_read(VOID *bufferAddr,UINT32 *bufferSize);
/**
 * @brief 写入RS485_2队列
 * 
 * @param bufferAddr 数据指针
 * @param bufferSize 数据大小
 * @return UINT32 0 成功
 */
UINT32 rs485_2_write(VOID *bufferAddr,UINT32 bufferSize);
/**
 * @brief 读取RS485_2队列数据
 * 
 * @param bufferAddr 数据指针
 * @param bufferSize 数据大小
 * @return UINT32 0 成功
 */
UINT32 rs485_2_read(VOID *bufferAddr,UINT32 *bufferSize);
/**
 * @brief 写入RS485_3队列
 * 
 * @param bufferAddr 数据指针
 * @param bufferSize 数据大小
 * @return UINT32 0 成功
 */
UINT32 rs485_3_write(VOID *bufferAddr,UINT32 bufferSize);
/**
 * @brief 读取RS485_3队列数据
 * 
 * @param bufferAddr 数据指针
 * @param bufferSize 数据大小
 * @return UINT32 0 成功
 */
UINT32 rs485_3_read(VOID *bufferAddr,UINT32 *bufferSize);


#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_ADAPTER_H__ */
