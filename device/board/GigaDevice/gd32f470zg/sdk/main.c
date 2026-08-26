#include "los_config.h"
#include "los_debug.h"
#include "los_interrupt.h"
#include "los_task.h"
#include "los_tick.h"

#include "uart.h"

#if defined(LOSCFG_SUPPORT_LITTLEFS)
#include "lfs_adapter.h"
#endif

#if (LOSCFG_FILE_SYSTEM_TEST == 1) && defined(LOSCFG_SUPPORT_LITTLEFS)
#include "file_test.h"
#endif

#if (LOSCFG_ETHERNET_ENABLE == 1)
#include "lwip_adapter.h"
#endif

void init_hw(void)
{
    // 初始化GPIO
    init_periph_gpio();
}

void init_hwi(void)
{
	// 初始化按键
	init_periph_key();
}

int main(void)
{
	UINT32 ret;   
    systick_config(); 
	uart_init();
    // 初始化硬件
    init_hw();
    
	//内核初始化
    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("LiteOS kernel init failed! ERROR: 0x%x\n", ret);
        while(1){}
    }
    uart_irq_register();

#if defined(LOSCFG_SUPPORT_LITTLEFS)
    lfs_init();
#endif
#if (LOSCFG_FILE_SYSTEM_TEST == 1) && defined(LOSCFG_SUPPORT_LITTLEFS)
    file_system_test();
#endif
    printf("Open Harmony 4.1.1 start ...\r\n\r\n");
    // 初始化硬件中断
    init_hwi();

    extern void OHOS_SystemInit(void);
    OHOS_SystemInit();

#if (LOSCFG_ETHERNET_ENABLE == 1)
    enet_adapter_init(NULL);
#endif

#if (LOSCFG_USE_SHELL == 1)
    ret = LosShellInit();
    if (ret != LOS_OK) {
        printf("LosShellInit failed! ERROR: 0x%x\n", ret);
    }
#endif

    LOS_Start();	
	while (1){}
}
