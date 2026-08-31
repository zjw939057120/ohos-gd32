#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "sys_gpio.h"

static void *thread_di_task(unsigned int arg)
{
	while(1) {
		uint8_t di1 = di_read(DI1_INDEX);
		uint8_t di2 = di_read(DI2_INDEX);
		uint8_t di3 = di_read(DI3_INDEX);
		uint8_t di4 = di_read(DI4_INDEX);
		uint8_t di5 = di_read(DI5_INDEX);
		uint8_t di6 = di_read(DI6_INDEX);
		uint8_t di = di1 | (di2  << 1) | (di3 << 2) | (di4 << 3) | (di5 << 4) | (di6 << 5);
		printf("di: %d\r\n", di);
		LOS_TaskDelay(10000);// 10秒后读取一次
	}

    return NULL;
}

static void di_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_di_task,
		.uwStackSize = 0x1000,
		.pcName = "diTask",
		.usTaskPrio = 6,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate diTask failed!\n");
		return;
	}
}

APP_FEATURE_INIT(di_task_init);
