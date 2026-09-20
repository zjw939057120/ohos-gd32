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
 * Hello World sample application for GD32F470ZI.
 * Prints "Hello World!" on the debug console (USART0, PA9 TX / PA10 RX) once
 * per second.
 *
 * The LiteOS-M tick rate is 100 Hz (LOSCFG_BASE_CORE_TICK_PER_SECOND), so
 * 1000 ms = 100 ticks.
 *
 * APP_FEATURE_INIT() places HelloWorldEntry into the .zinitcall.app.feature
 * init table. The bootstrap service (bootstrap_service.c, itself registered
 * via SYS_SERVICE_INIT) iterates that table on BOOT_SYS_COMPLETED -- reached
 * through OHOS_SystemInit() -> SAMGR_Bootstrap() at boot -- so the entry needs
 * no manual call.
 */

#include <stdio.h>
#include "ohos_init.h"
#include "los_task.h"

#define HELLO_TASK_PRIO       11
#define HELLO_TASK_STACK_SIZE LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
#define HELLO_INTERVAL_TICKS  1000U /* 1000 ms at the 100 Hz tick rate */

static VOID *HelloTask(UINT32 arg)
{
    (void)arg;
    while (1) {
        printf("Hello World!\n");
        (void)LOS_TaskDelay(HELLO_INTERVAL_TICKS);
    }
    return NULL;
}

static void HelloWorldEntry(void)
{
    UINT32 taskId;
    TSK_INIT_PARAM_S taskParam = {0};

    taskParam.pfnTaskEntry = HelloTask;
    taskParam.uwStackSize  = HELLO_TASK_STACK_SIZE;
    taskParam.pcName       = "HelloTask";
    taskParam.usTaskPrio   = HELLO_TASK_PRIO;

    if (LOS_TaskCreate(&taskId, &taskParam) != LOS_OK) {
        printf("[HelloWorld] Failed to create HelloTask!\n");
    }
}

SYS_RUN(HelloWorldEntry);
