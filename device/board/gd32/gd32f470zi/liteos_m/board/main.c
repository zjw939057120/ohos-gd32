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
#include "lfs_adapter.h"
#include "queue_adapter.h"
#include "rs485_adapter.h"
#include "rtc_adapter.h"
#include "spi0_adapter.h"
#include "spi1_adapter.h"
#include "uart.h"
#include "watchdog_adapter.h"

unsigned int LosShellInit(void);

/*
 * OHOS bootstrap entry. Traverses the .zinitcall.* init tables (CORE_INIT,
 * SYS_SERVICE_INIT, SYS_RUN, ...) so vendor/application code registered via
 * those macros starts automatically. Declared here because system_init.c
 * does not expose a public header for it.
 */
extern void OHOS_SystemInit(void);


/*
 * core_main.h defines MODULE_INIT / MODULE_CALL / MODULE_BEGIN / MODULE_END
 * (base/startup/bootstrap_lite/services/source/core_main.h). MODULE_INIT(...)
 * expands to a do-while over five MODULE_CALL(name, step) invocations, and
 * each MODULE_CALL references `extern __zinitcall_<name>_start/end` declared
 * in MODULE_BEGIN/MODULE_END. Without the include the preprocessor leaves
 * MODULE_INIT(app_service) as a plain function call and gcc reports
 * "'app_service' undeclared (first use in this function)". system_init.c
 * pulls core_main.h via a same-directory relative include; board/main.c
 * must include it via the bootstrap include path that gn adds through
 * board_include_dirs in liteos_m/config.gni.
 *
 * NOTE: core_main.h uses `InitCall` (typedef void (*InitCall)(void) from
 * commonlibrary/utils_lite/include/ohos_init.h) but does NOT include
 * ohos_init.h itself -- system_init.c gets it for free because it #includes
 * <ohos_init.h> before core_main.h. board/main.c must include both, in that
 * order: <ohos_init.h> first to introduce the typedef, then "core_main.h"
 * which uses it.
 */
#include <ohos_init.h>
#include "core_main.h"

void init_hw(void)
{
    // 初始化 TLC2543 SPI 通信
    init_tlc2543_spi();
    // 初始化 AD5318 SPI 通信
    init_ad5318_spi();
}

void init_hwi(void)
{
    // 初始化队列
    init_queue();
    // 注册RS485接收中断
    rs485_irq_register();
     // 初始化RTC
    init_rtc();
    // 初始化看门狗
    init_watchdog();
    // 初始化按键
	init_key();
}
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
#if (LOSCFG_USE_SHELL == 1)
    // 初始化UART
    UartInit();
#endif
    // 初始化RS485
    init_rs485();
    // 初始化硬件外设
    init_hw();

    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("LiteOS kernel init failed! ERROR: 0x%x\n", ret);
        goto EXIT;
    }

#if (LOSCFG_USE_SHELL == 1)
    // 注册UART接收中断
    UartRxIrqRegister();
#endif
    // 初始化硬件中断
    init_hwi();
    // 初始化LITTLEFS
#if (LOSCFG_SUPPORT_LITTLEFS == 1)
    lfs_init();
#endif
    // 初始化以太网
#if (LOSCFG_NET_LWIP == 1)
    enet_adapter_init(NULL);
#endif
    // 初始化Shell
#if (LOSCFG_USE_SHELL == 1)
    ret = LosShellInit();
    if (ret != LOS_OK) {
        printf("LosShellInit failed! ERROR: 0x%x\n", ret);
    }
#endif

    // 初始化应用服务
    MODULE_INIT(app_service);
    // 初始化应用特征
    MODULE_INIT(app_feature);
    /* Start OHOS auto-init (runs SYS_RUN app entries, e.g. the LED blink). */
    OHOS_SystemInit();

    LOS_Start();

EXIT:
    while (1) {
        __asm volatile("wfi");
    }
}
