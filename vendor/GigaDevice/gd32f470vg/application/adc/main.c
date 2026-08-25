#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"

#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 6 /* 值越大，优先级越高 */

uint16_t adc_value[2];

void adc_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);

    /* ADC mode config */
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC contineous function disable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan mode disable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 2);
    /* ADC routine channel config */
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_15);
    adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_17, ADC_SAMPLETIME_15);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

    /* ADC temperature and Vref enable */
    adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH, ENABLE);

    /* ADC DMA function enable */
    adc_dma_request_after_last_enable(ADC0);
    adc_dma_mode_enable(ADC0);

    /* enable ADC interface */
    adc_enable(ADC0);
    /* wait for ADC stability */
    LOS_TaskDelay(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    /* enable ADC software trigger */
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}

void dma_config(void)
{
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);

    /* ADC_DMA_channel configuration */
    dma_single_data_parameter_struct dma_single_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA1, DMA_CH0);

    /* initialize DMA single data mode */
    dma_single_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)(adc_value);
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = 2;
    dma_single_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_single_data_mode_init(DMA1, DMA_CH0, &dma_single_data_parameter);
    dma_channel_subperipheral_select(DMA1, DMA_CH0, DMA_SUBPERI0);

    /* enable DMA circulation mode */
    dma_circulation_enable(DMA1, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA1, DMA_CH0);
}

VOID adc_task(VOID)
{
    float temperature;
    float vref_value;

    adc_config();
    dma_config();

    while (1)
    {
        temperature = (1.43f - adc_value[0] * 3.3f / 4096) * 1000 / 4.3f + 25;
        vref_value = (adc_value[1] * 3.3f / 4096);

        printf("the temperature data is %2.0f degrees Celsius\r\n", temperature);
        printf("the reference voltage data is %5.3fV \r\n", vref_value);

        LOS_TaskDelay(1000);
    }
}

VOID adc_task_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)adc_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "adc_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK)
    {
        printf("adc_task create failed.\n");
    }
}

APP_FEATURE_INIT(adc_task_handler);
