#include <stdio.h>
#include <ohos_init.h>
#include "los_task.h"

#define LED_CLK RCU_GPIOD
#define LED_PORT GPIOD
#define RED_LED_PIN GPIO_PIN_14
#define GREEN_LED_PIN GPIO_PIN_15
#define BLUE_LED_PIN GPIO_PIN_13

#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 6 /* 值越大，优先级越高 */

typedef enum
{
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
} Led_Color;

/* 初始化gpio */
static void led_init(void)
{
    rcu_periph_clock_enable(LED_CLK);

    gpio_mode_set(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RED_LED_PIN);
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RED_LED_PIN);

    gpio_bit_set(LED_PORT, RED_LED_PIN);

    gpio_mode_set(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GREEN_LED_PIN);
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GREEN_LED_PIN);

    gpio_bit_set(LED_PORT, GREEN_LED_PIN);

    gpio_mode_set(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLUE_LED_PIN);
    gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BLUE_LED_PIN);

    gpio_bit_set(LED_PORT, BLUE_LED_PIN);
}

static void LedCtrl(Led_Color color, uint8_t state)
{
    switch (color)
    {
    case LED_RED:
        gpio_bit_write(LED_PORT, RED_LED_PIN, state);
        break;
    case LED_GREEN:
        gpio_bit_write(LED_PORT, GREEN_LED_PIN, state);
        break;
    case LED_BLUE:
        gpio_bit_write(LED_PORT, BLUE_LED_PIN, state);
        break;
    default:
        break;
    }
}

VOID led_task(VOID)
{
    led_init();

    while (1)
    {
        LedCtrl(LED_RED, 0);
        LedCtrl(LED_GREEN, 1);
        LedCtrl(LED_BLUE, 1);
        LOS_TaskDelay(500);
        LedCtrl(LED_RED, 1);
        LedCtrl(LED_GREEN, 0);
        LedCtrl(LED_BLUE, 1);
        LOS_TaskDelay(500);
        LedCtrl(LED_RED, 1);
        LedCtrl(LED_GREEN, 1);
        LedCtrl(LED_BLUE, 0);
        LOS_TaskDelay(500);
    }
}

VOID led_task_handler(VOID)
{
    UINT32 ret;
    UINT32 g_task_id;
    TSK_INIT_PARAM_S g_task = {0};

    g_task.pfnTaskEntry = (TSK_ENTRY_FUNC)led_task;
    g_task.uwStackSize = TASK_STACK_SIZE;
    g_task.pcName = "led_task";
    g_task.usTaskPrio = TASK_PRIORITY;
    ret = LOS_TaskCreate(&g_task_id, &g_task);
    if (ret != LOS_OK)
    {
        printf("led_task create failed.\n");
    }
}

APP_FEATURE_INIT(led_task_handler);
