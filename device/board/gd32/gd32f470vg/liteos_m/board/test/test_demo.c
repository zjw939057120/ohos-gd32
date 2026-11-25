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

#include <stdio.h>
#include "los_task.h"
#include "los_debug.h"

static void SampleTaskEntry(VOID)
{
    while (1) {
        printf("Hello GD32F470VG LiteOS-M!\n");
        (void)LOS_TaskDelay(1000); /* 1000 ticks */
    }
}

unsigned int LosAppInit(VOID)
{
    unsigned int ret;
    UINT32 taskId;
    TSK_INIT_PARAM_S taskInitParam;

    ret = (unsigned int)memset_s(&taskInitParam, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != LOS_OK) {
        return ret;
    }

    taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)SampleTaskEntry;
    taskInitParam.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskInitParam.pcName       = "SampleTask";
    taskInitParam.usTaskPrio   = 10;

    ret = LOS_TaskCreate(&taskId, &taskInitParam);
    if (ret != LOS_OK) {
        printf("Create SampleTask failed! ERROR: 0x%x\n", ret);
    }

    return ret;
}
