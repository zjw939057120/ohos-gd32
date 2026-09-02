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

#include "queue_adapter.h"
#include "los_queue.h"
#include "los_task.h"


UINT32 rs485_1 = 0;
UINT32 rs485_2 = 0;
UINT32 rs485_3 = 0;

UINT32 queue_create(CHAR *queueName,UINT16 len,UINT32 *queueID,UINT32 flags,UINT16 maxMsgSize){
	return LOS_QueueCreate(queueName,len,queueID,flags,maxMsgSize);
}

UINT32 queue_write(UINT32 queueID,VOID *bufferAddr,UINT32 bufferSize){
	return LOS_QueueWriteCopy(queueID,bufferAddr,bufferSize,0);
}

UINT32 queue_read(UINT32 queueID,VOID *bufferAddr,UINT32 *bufferSize){
	return LOS_QueueReadCopy(queueID,bufferAddr,bufferSize,LOS_WAIT_FOREVER);
}

void queue_init(void)
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

UINT32 rs485_1_write(VOID *bufferAddr,UINT32 bufferSize){
	return queue_write(rs485_1,bufferAddr,bufferSize);
}

UINT32 rs485_1_read(VOID *bufferAddr,UINT32 *bufferSize){
	return queue_read(rs485_1,bufferAddr,bufferSize);
}

UINT32 rs485_2_write(VOID *bufferAddr,UINT32 bufferSize){
	return queue_write(rs485_2,bufferAddr,bufferSize);
}

UINT32 rs485_2_read(VOID *bufferAddr,UINT32 *bufferSize){
	return queue_read(rs485_2,bufferAddr,bufferSize);
}

UINT32 rs485_3_write(VOID *bufferAddr,UINT32 bufferSize){
	return queue_write(rs485_3,bufferAddr,bufferSize);
}

UINT32 rs485_3_read(VOID *bufferAddr,UINT32 *bufferSize){
	return queue_read(rs485_3,bufferAddr,bufferSize);
}
