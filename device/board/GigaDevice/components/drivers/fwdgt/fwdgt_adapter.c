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

#include "fwdgt_adapter.h"
#include "los_task.h"

#define TASK_STACK_SIZE 2048
#define TASK_PRIORITY 3 /* 值越大，优先级越高 */

VOID fwdgt_task(VOID)
{
    while (1)
    {
		fwdgt_counter_reload();
        LOS_TaskDelay(3000);
    }
}

void init_fwdgt(void)
{
    /* enable IRC32K */
    rcu_osci_on(RCU_IRC32K);
    
    /* wait till IRC32K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC32K)){
    }

    /* confiure FWDGT counter clock: 2500 * 64 / 32000 = 5s */
    fwdgt_config(2500, FWDGT_PSC_DIV64);
    
    fwdgt_enable();

    /* check if the system has resumed from FWDGT reset */
    if (RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)){
        /* clear the FWDGT reset flag */
        rcu_all_reset_flag_clear();
	}

    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)fwdgt_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "fwdgt_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK)
    {
        printf("fwdgt_task create failed.\n");
    }
}