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
 * LED blink application for GD32F470ZG, registered through the OHOS bootstrap.
 *
 * GPIO allocation (see docs/io_allocation.md):
 *   PA8  -> LED_GREEN  (active high, push-pull output)
 *   PE3  -> LED_BLUE   (active high, push-pull output)
 * Both LEDs are driven through the board LED driver (led.h: LedInit/LedOn/LedOff)
 * and blink together at 1 Hz: 500 ms on + 500 ms off.
 *
 * The LiteOS-M tick rate is 100 Hz (LOSCFG_BASE_CORE_TICK_PER_SECOND), so
 * 500 ms = 50 ticks; LOS_TaskDelay(50) on + LOS_TaskDelay(50) off gives one
 * cycle/second.
 *
 * APP_FEATURE_INIT() places LedExampleEntry into the .zinitcall.app.feature
 * init table. The bootstrap service (bootstrap_service.c, itself registered
 * via SYS_SERVICE_INIT) iterates that table on BOOT_SYS_COMPLETED -- reached
 * through OHOS_SystemInit() -> SAMGR_Bootstrap() at boot -- so the entry needs
 * no manual call, and it does not clash with the kernel testsuites' LosAppInit().
 */

#include <stdio.h>
#include "ohos_init.h"
#include "los_task.h"
#include "led.h"

#define LED_TASK_PRIO       11
#define LED_TASK_STACK_SIZE LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE
#define LED_BLINK_TICKS     500U /* 500 ms at the 100 Hz tick rate */

/* LED1 (PA8, LED_GREEN) + LED2 (PE3, LED_BLUE) blink at 1 Hz, active high. */
static VOID *LedTask(UINT32 arg)
{
    (void)arg;
    LedInit();
    while (1) {
        LedOn();                        /* high level lights the LEDs */
        (void)LOS_TaskDelay(LED_BLINK_TICKS);
        LedOff();
        (void)LOS_TaskDelay(LED_BLINK_TICKS);
    }
    return NULL;
}

static void LedExampleEntry(void)
{
    UINT32 taskId;
    TSK_INIT_PARAM_S taskParam = {0};

    taskParam.pfnTaskEntry = LedTask;
    taskParam.uwStackSize  = LED_TASK_STACK_SIZE;
    taskParam.pcName       = "LedTask";
    taskParam.usTaskPrio   = LED_TASK_PRIO;

    printf("[LedExample] PA8/PE3 blink @ 1Hz\n");
    if (LOS_TaskCreate(&taskId, &taskParam) != LOS_OK) {
        printf("[LedExample] Failed to create LedTask!\n");
    }
}

SYS_RUN(LedExampleEntry);
