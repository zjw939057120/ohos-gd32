#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "sys_gpio.h"


#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6       /* 值越大，优先级越高 */

VOID sysled_task(VOID)
{
    while (1) {
		led_on(RUNSTA_LED_INDEX);
        LOS_TaskDelay(1000);

		led_off(RUNSTA_LED_INDEX);
        LOS_TaskDelay(1000);
    }
}

VOID sysled_task_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)sysled_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "sysled_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK) {
        printf("sysled_task create failed.\n");
    }	
}

APP_FEATURE_INIT(sysled_task_handler);
