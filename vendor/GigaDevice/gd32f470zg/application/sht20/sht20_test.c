
#include "bsp_iic.h"
#include "los_task.h"
#include "stdio.h"
#include <ohos_init.h>
#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 4 


VOID sht20_task(VOID)
{
   iic_gpio_config();
   while(1){
    LOS_TaskDelay(3000);
    //printf("temp test2\n");
    printf("temp = %f\r\n",SHT20_receive_data(0xf3));
    printf("humi = %f\r\n",SHT20_receive_data(0xf5));
   }
}

VOID sht20Test(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};
    printf("sht20_task 1\r\n");

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)sht20_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "sht20_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK)
    {
        printf("led_task create failed.\n");
    }
    printf("sht20_task 2\n");

}

APP_FEATURE_INIT(sht20Test);