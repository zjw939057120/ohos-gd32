
#include "bsp_spi.h"
#include <stdlib.h>
#include "los_task.h"
#include "stdio.h"
#include <ohos_init.h>
#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 4 


VOID w25q64_task(VOID)
{
   unsigned char buff[10] = {0};
   int i = 0;
   char str[5];
	w25q64_init_config();
	
	printf("ID = %X\r\n",W25Q64_readID());
   
 
   while(1){
      LOS_TaskDelay(3000);
      W25Q64_read(buff, i*5, i*5+5);
      printf("buff = %s\r\n",buff);
      itoa(i, str, 10);
      W25Q64_write((uint8_t* )str, i*5, i*5+5);
      W25Q64_read(buff, i*5, i*5+5);
      printf("buff = %s\r\n",buff);
      if(i >=100){
         i = 0;
      }else{
         i++;
      } 
   }
}

VOID w25q64Test(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};
    printf("tw25q64_task 1\r\n");

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)w25q64_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "w25q64_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK)
    {
        printf("w25q64_task create failed.\n");
    }
    printf("tw25q64_task 2\n");

}

APP_FEATURE_INIT(w25q64Test);