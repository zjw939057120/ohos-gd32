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
    // 初始化 TLC2543 SPI 通信
    tlc2543_spi_init();
    // 初始化 AD5318 SPI 通信
    ad5318_spi_init();
}
void init_hwi(void)
{
    // 初始化队列
    queue_init();
    // 注册UART中断
    uart_irq_register();
     // 初始化RTC
    init_rtc();
    // 初始化看门狗
    init_watchdog();
    // 初始化按键
	init_periph_key();
}

int main(void)
{
	UINT32 ret;
    // 初始化系统时钟
    systick_config();
    // 初始化UART口
	uart_init();
    // 初始化硬件
    init_hw();
    
	//内核初始化
    ret = LOS_KernelInit();
    if (ret != LOS_OK) {
        printf("LiteOS kernel init failed! ERROR: 0x%x\n", ret);
        while(1){}
    }
    // 初始化硬件中断，内核初始化完成后调用
    init_hwi();
    
#if defined(LOSCFG_SUPPORT_LITTLEFS)
    lfs_init();
#endif
#if (LOSCFG_FILE_SYSTEM_TEST == 1) && defined(LOSCFG_SUPPORT_LITTLEFS)
    file_system_test();
#endif
    printf("%s start ... build at %s\r\n", OHOS_VERSION, OHOS_BUILD_DATETIME);

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
