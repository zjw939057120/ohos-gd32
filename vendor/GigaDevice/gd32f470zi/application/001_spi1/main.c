#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"
#include "spi1_adapter.h"

#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6                   /* 值越大，优先级越高 */

static void *thread_adc_task(unsigned int arg)
{
uint16_t adc_value = 0;
float voltage = 0.0f;
uint8_t channel = 0;

while(1) {
    // 读一次，丢弃数据（触发当前通道的转换）
    tlc2543_read_adc(channel);
    
    // 等待一小段时间，让 ADC 有足够的时间完成转换和内部采样
    // TLC2543 转换时间约 10us，但考虑到切换通道的稳定时间，建议 1-5ms
    LOS_TaskDelay(5); 
    
    // 再读一次，这次读出来的才是当前通道的真实数据
    adc_value = tlc2543_read_adc(channel);
    
    // 将 12位 ADC 值转换为实际电压 (参考电压 4.55V)
    voltage = ((float)adc_value / 4095.0f) * 4.55f;
    
    printf("ADC Channel %d: Raw: %d | Voltage: %.3f V\r\n", channel, adc_value, voltage);

    channel++;
    if(channel > 10) {
        channel = 0;
    }
    
    LOS_TaskDelay(10000);// 10秒后切换下一个通道
}

    return NULL;
}

UINT32 adc_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_adc_task,
		.uwStackSize = TASK_STACK_SIZE,
		.pcName = "adcTask",
		.usTaskPrio = TASK_PRIORITY,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate adcTask failed!\n");
		return -1;
	}
	return taskID;
}

static void *thread_dac_task(unsigned int arg)
{
    // 初始化SPI_DAC
    ad5318_spi_init();
    uint8_t channel = 0;
    uint16_t hi = 1023;
    uint16_t lo = hi/2;
    while(1) {
        ad5318_write_dac(channel, lo);
        LOS_TaskDelay(3000); // 3秒延时
        ad5318_write_dac(channel, hi);
        printf("DAC Channel %d set to %u\r\n", channel, hi);
        channel++;
        if(channel > 7) {
            channel = 0;
        }
        LOS_TaskDelay(3000); // 3秒延时
    }
    return NULL;
}

UINT32 dac_task_init(void)
{
	UINT32 taskID;
	TSK_INIT_PARAM_S stTask = {
		.pfnTaskEntry = thread_dac_task,
		.uwStackSize = TASK_STACK_SIZE,
		.pcName = "dacTask",
		.usTaskPrio = TASK_PRIORITY,
	};
	if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate dacTask failed!\n");
		return -1;
	}
	return taskID;
}

static void spi1_task_init(void)
{
    adc_task_init();
    dac_task_init();
}

APP_FEATURE_INIT(spi1_task_init);
