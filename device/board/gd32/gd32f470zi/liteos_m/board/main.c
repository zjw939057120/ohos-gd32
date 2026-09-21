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
#include "los_config.h"
#include "uart.h"
#include "los_debug.h"
#include "gpio_adapter.h"

unsigned int LosShellInit(void);

/*
 * OHOS bootstrap entry. Traverses the .zinitcall.* init tables (CORE_INIT,
 * SYS_SERVICE_INIT, SYS_RUN, ...) so vendor/application code registered via
 * those macros starts automatically. Declared here because system_init.c
 * does not expose a public header for it.
 */
extern void OHOS_SystemInit(void);

/*****************************************************************************
 Function    : main
 Description : Main function entry
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT int main(void)
{
    unsigned int ret;
    // 初始化GPIO
    init_gpio();

    // 初始化UART
    UartInit();

    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("LiteOS kernel init failed! ERROR: 0x%x\n", ret);
        goto EXIT;
    }
    
    // 注册UART接收中断
    Uart0RxIrqRegister();

#if (LOSCFG_USE_SHELL == 1)
    ret = LosShellInit();
    if (ret != LOS_OK) {
        printf("LosShellInit failed! ERROR: 0x%x\n", ret);
    }
#endif

    /* Start OHOS auto-init (runs SYS_RUN app entries, e.g. the LED blink). */
    OHOS_SystemInit();

    LOS_Start();

EXIT:
    while (1) {
        __asm volatile("wfi");
    }
}
