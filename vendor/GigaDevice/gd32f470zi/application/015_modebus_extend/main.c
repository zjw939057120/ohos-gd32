#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "uart.h"
#include "queue_adapter.h"

#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6                   /* 值越大，优先级越高 */

static void *thread_modebus_extend_task(unsigned int arg)
{
	UINT32 ret = 0;
	uint8_t bufferAddr[MAX_MSG_SIZE];  /* 接收缓冲区 */
	UINT32 bufferSize = 0;  /* 接收缓冲区大小 */
	
	while(1) {
		bufferSize = MAX_MSG_SIZE;
		ret = rs485_3_mq_recv(bufferAddr,&bufferSize);
		if(ret != LOS_OK) {
			LOS_TaskDelay(100);
			continue;
		}
		rs485_3_send(bufferAddr,bufferSize);
	}

    return NULL;
}

static void modebus_extend_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_modebus_extend_task,
		.uwStackSize = TASK_STACK_SIZE,
		.pcName = "modebusExtendTask",
		.usTaskPrio = TASK_PRIORITY,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate modebusExtendTask failed!\n");
		return;
	}
}

APP_FEATURE_INIT(modebus_extend_task_init);
