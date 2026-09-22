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

#include "queue_adapter.h"
#include "los_interrupt.h"
#include "los_event.h"
#include "pin_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

UINT32 rs485_1 = 0;
UINT32 rs485_2 = 0;
UINT32 rs485_3 = 0;

UINT32 queue_create(CHAR *queueName,UINT16 len,UINT32 *queueID,UINT32 flags,UINT16 maxMsgSize){
	return LOS_QueueCreate(queueName,len,queueID,flags,maxMsgSize);
}

UINT32 queue_send(UINT32 queueID,VOID *bufferAddr,UINT32 bufferSize){
	return LOS_QueueWriteCopy(queueID,bufferAddr,bufferSize,0);
}

UINT32 queue_recv(UINT32 queueID,VOID *bufferAddr,UINT32 *bufferSize){
	return LOS_QueueReadCopy(queueID,bufferAddr,bufferSize,LOS_WAIT_FOREVER);
}

void init_queue(void)
{
    UINT32 ret = 0;
    ret = queue_create("rs485_1", MAX_QUEUE_LEN, &rs485_1, 0, MAX_MSG_SIZE);
    if(ret != LOS_OK){
        printf("rs485_1 queue create failed\n");
    }
    ret = queue_create("rs485_2", MAX_QUEUE_LEN, &rs485_2, 0, MAX_MSG_SIZE);
    if(ret != LOS_OK){
        printf("rs485_2 queue create failed\n");
    }
    ret = queue_create("rs485_3", MAX_QUEUE_LEN, &rs485_3, 0, MAX_MSG_SIZE);
    if(ret != LOS_OK){
        printf("rs485_3 queue create failed\n");
    }
}

UINT32 rs485_1_mq_send(VOID *bufferAddr,UINT32 bufferSize){
	return queue_send(rs485_1,bufferAddr,bufferSize);
}

UINT32 rs485_1_mq_recv(VOID *bufferAddr,UINT32 *bufferSize){
	return queue_recv(rs485_1,bufferAddr,bufferSize);
}

UINT32 rs485_2_mq_send(VOID *bufferAddr,UINT32 bufferSize){
	return queue_send(rs485_2,bufferAddr,bufferSize);
}

UINT32 rs485_2_mq_recv(VOID *bufferAddr,UINT32 *bufferSize){
	return queue_recv(rs485_2,bufferAddr,bufferSize);
}

UINT32 rs485_3_mq_send(VOID *bufferAddr,UINT32 bufferSize){
	return queue_send(rs485_3,bufferAddr,bufferSize);
}

UINT32 rs485_3_mq_recv(VOID *bufferAddr,UINT32 *bufferSize){
	return queue_recv(rs485_3,bufferAddr,bufferSize);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
