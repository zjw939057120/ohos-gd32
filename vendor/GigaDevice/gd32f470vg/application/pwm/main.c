#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"

#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 6 /* 值越大，优先级越高 */

#define LED_PIN GPIO_PIN_13
#define LED_PORT GPIOD
#define LED_CLK RCU_GPIOD

#define BLINK_TRUE 1
#define BLINK_FALSE 0

#define DUTY_COUNT 99999

void pwm_init(void)
{
    rcu_periph_clock_enable(LED_CLK);

    gpio_mode_set(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN);
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_PIN);
    gpio_af_set(LED_PORT, GPIO_AF_2, LED_PIN);

    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER3); // 使能 TIMER3 时钟

    timer_deinit(TIMER3);

    /* 定时器配置 */
    timer_initpara.prescaler = 119;                     /* 对时钟进行预分频，240Mhz, 分下来是2Mhz */
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;    /* 计数的对齐方式，通常有上升沿或下降沿对齐 */
    timer_initpara.counterdirection = TIMER_COUNTER_UP; /* 向上计数还是向下计数 */
    timer_initpara.period = DUTY_COUNT;                 /* 这个参数设置定时器的周期值，通常是计数器达到该值后会产生一个中断或重置 */
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;    /* 参数用于设置定时器的时钟分频 */
    timer_initpara.repetitioncounter = 0;               /* 设置定时器的重复计数，提前装入的值，为了特定频率 */
    timer_init(TIMER3, &timer_initpara);

    /* PWM模式下配置 CH0、CH1 和 CH2 */
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;          /* 输出状态 */
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;       /* 反向输出状态 */
    timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;     /* 输出极性 */
    timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;   /* 反向输出极性 */
    timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;   /* 输出空闲状态 */
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW; /* 反向输出空闲状态 */

    // 配置 TIMER0 通道输出功能
    timer_channel_output_config(TIMER3, TIMER_CH_1, &timer_ocintpara);

    /* CH0 以 PWM 模式工作，占空比为 0% */
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_1, 0);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER3, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* 自动重装载预装载使能 */
    timer_auto_reload_shadow_enable(TIMER3);
    /* 定时器使能 */
    timer_enable(TIMER3);
}

/* 设置占空比 */
void pwm_duty(int duty)
{
    if (duty > DUTY_COUNT)
    {
        return;
    }

    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_1, duty);
}

VOID pwm_task(VOID)
{
    int i = 0;
    uint8_t flg = BLINK_FALSE;

    pwm_init();

    while (1)
    {
        if (i > 25000)
        {
            flg = BLINK_TRUE;
        }
        if (i < 1)
        {
            flg = BLINK_FALSE;
        }

        if (flg == BLINK_FALSE)
        {
            i = i + 50;
        }
        if (flg == BLINK_TRUE)
        {
            i = i - 50;
        }
        pwm_duty(i);
        LOS_TaskDelay(50);
    }
}

VOID pwm_task_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)pwm_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "pwm_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK)
    {
        printf("pwm_task create failed.\n");
    }
}

APP_FEATURE_INIT(pwm_task_handler);
