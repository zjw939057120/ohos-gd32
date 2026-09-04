/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
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

#ifndef __GPIO_ADAPTER_H__
#define __GPIO_ADAPTER_H__

#include "gd32f4xx.h"
#include "gd32f4xx_gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// RS485_1_LED~RS485_3_LED LED灯定义
// RS485_1_LED PA10
#define RS485_1_LED_INDEX           0U
#define RS485_1_LED_GPIO_CLK        RCU_GPIOA
#define RS485_1_LED_GPIO_PORT		GPIOA
#define RS485_1_LED_PIN				GPIO_PIN_10
// RS485_2_LED PC8
#define RS485_2_LED_INDEX           1U
#define RS485_2_LED_GPIO_CLK        RCU_GPIOC
#define RS485_2_LED_GPIO_PORT		GPIOC
#define RS485_2_LED_PIN				GPIO_PIN_8
// RS485_3_LED PA11
#define RS485_3_LED_INDEX           2U
#define RS485_3_LED_GPIO_CLK        RCU_GPIOA
#define RS485_3_LED_GPIO_PORT		GPIOA
#define RS485_3_LED_PIN				GPIO_PIN_11

// RUNSTA_LED LED灯定义
// RUNSTA_LED PA9
#define RUNSTA_LED_INDEX            3U
#define RUNSTA_LED_GPIO_CLK         RCU_GPIOA
#define RUNSTA_LED_GPIO_PORT		GPIOA
#define RUNSTA_LED_PIN				GPIO_PIN_9

// RS485_1_EN~RS485_3_EN使能引脚定义
// RS485_1_EN PB9
#define RS485_1_EN_INDEX            4U
#define RS485_1_EN_GPIO_CLK         RCU_GPIOB
#define RS485_1_EN_GPIO_PORT        GPIOB
#define RS485_1_EN_GPIO_PIN			GPIO_PIN_9
// RS485_2_EN PB8
#define RS485_2_EN_INDEX            5U
#define RS485_2_EN_GPIO_CLK         RCU_GPIOB
#define RS485_2_EN_GPIO_PORT        GPIOB
#define RS485_2_EN_GPIO_PIN			GPIO_PIN_8
// RS485_3_EN PD7
#define RS485_3_EN_INDEX            6U
#define RS485_3_EN_GPIO_CLK         RCU_GPIOD
#define RS485_3_EN_GPIO_PORT        GPIOD
#define RS485_3_EN_GPIO_PIN			GPIO_PIN_7

// USER_KEY按键定义
// USER_KEY PA12
#define USER_KEY_INDEX              7U
#define USER_KEY_PIN                GPIO_PIN_12
#define USER_KEY_GPIO_PORT          GPIOA
#define USER_KEY_GPIO_CLK           RCU_GPIOA
#define USER_KEY_EXTI_LINE          EXTI_12
#define USER_KEY_EXTI_PORT_SOURCE   EXTI_SOURCE_GPIOA
#define USER_KEY_EXTI_PIN_SOURCE    EXTI_SOURCE_PIN12
#define USER_KEY_EXTI_IRQn          EXTI10_15_IRQn

//DO1~DO3引脚定义
//DO1 PB1
#define DO1_INDEX                   8U
#define DO1_PIN		                GPIO_PIN_1
#define DO1_GPIO_CLK         	    RCU_GPIOB
#define DO1_GPIO_PORT		        GPIOB
//DO2 PA6
#define DO2_INDEX                   9U
#define DO2_PIN		                GPIO_PIN_6
#define DO2_GPIO_CLK         	    RCU_GPIOA
#define DO2_GPIO_PORT		        GPIOA
//DO3 PA5
#define DO3_INDEX                   10U
#define DO3_PIN		                GPIO_PIN_5
#define DO3_GPIO_CLK         	    RCU_GPIOA
#define DO3_GPIO_PORT		        GPIOA

//DI1~DI6引脚定义
//DI1 PF7
#define DI1_INDEX                   11U
#define DI1_PIN		                GPIO_PIN_7
#define DI1_GPIO_CLK         	    RCU_GPIOF
#define DI1_GPIO_PORT		        GPIOF
//DI2 PE6
#define DI2_INDEX                   12U
#define DI2_PIN		                GPIO_PIN_6
#define DI2_GPIO_CLK         	    RCU_GPIOE
#define DI2_GPIO_PORT		        GPIOE
//DI3 PE5
#define DI3_INDEX                   13U
#define DI3_PIN		                GPIO_PIN_5
#define DI3_GPIO_CLK         	    RCU_GPIOE
#define DI3_GPIO_PORT		        GPIOE
//DI4 PE4
#define DI4_INDEX                   14U
#define DI4_PIN		                GPIO_PIN_4
#define DI4_GPIO_CLK         	    RCU_GPIOE
#define DI4_GPIO_PORT		        GPIOE
//DI5 PE3
#define DI5_INDEX                   15U
#define DI5_PIN		                GPIO_PIN_3
#define DI5_GPIO_CLK         	    RCU_GPIOE
#define DI5_GPIO_PORT		        GPIOE
//DI6 PE2
#define DI6_INDEX                   16U
#define DI6_PIN		                GPIO_PIN_2
#define DI6_GPIO_CLK         	    RCU_GPIOE
#define DI6_GPIO_PORT		        GPIOE


/**
 * @brief GPIO引脚定义结构体
 * 
 * @param rcu RCU外设枚举值
 * @param gpio GPIO外设枚举值
 * @param pin GPIO引脚枚举值
 */
typedef struct {
    rcu_periph_enum rcu;
    uint32_t gpio;
    uint32_t pin;
} gpio_t;

/**
 * @brief 初始化GPIO外设
 * 
 */
void init_gpio();

/**
 * @brief 设置GPIO引脚为高电平
 * 
 * @param index GPIO引脚索引
 */
void gpio_high(uint8_t index);

/**
 * @brief 设置GPIO引脚为低电平
 * 
 * @param index GPIO引脚索引
 */
void gpio_low(uint8_t index);

/**
 * @brief 初始化GPIO引脚为LED灯
 * 
 * @param index GPIO引脚索引
 */
void init_bsp_led(uint8_t index);

/**
 * @brief 使能LED灯
 * 
 * @param index GPIO引脚索引
 */
void led_on(uint8_t index);

/**
 * @brief 禁用LED灯
 * 
 * @param index GPIO引脚索引
 */
void led_off(uint8_t index);

/**
 * @brief 切换GPIO引脚电平
 * 
 * @param index GPIO引脚索引
 */
void gpio_toggle(uint8_t index);

/**
 * @brief 设置DO引脚为高电平
 * 
 * @param index GPIO引脚索引
 */
void do_on(uint8_t index);

/**
 * @brief 设置DO引脚为低电平
 * 
 * @param index GPIO引脚索引
 */
void do_off(uint8_t index);

/**
 * @brief 读取DI引脚电平
 * 
 * @param index GPIO引脚索引
 * @return uint8_t DI引脚电平
 */
uint8_t di_read(uint8_t index);

/**
 * @brief 初始化RS485使能引脚
 * 
 * @param index GPIO引脚索引
 */
void rs485_en_init(uint8_t index);

/**
 * @brief 设置RS485使能引脚为高电平
 * 
 * @param high 是否为高电平
 */
void rs485_1_en(bool high);

/**
 * @brief 设置RS485使能引脚为低电平
 * 
 * @param high 是否为高电平
 */
void rs485_2_en(bool high);

/**
 * @brief 设置RS485使能引脚为低电平
 * 
 * @param high 是否为高电平
 */
void rs485_3_en(bool high);

/**
 * @brief 初始化LED灯
 * 
 */
void init_led();

/**
 * @brief 初始化DO引脚
 * 
 */
void init_do();

/**
 * @brief 初始化DI引脚
 * 
 */
void init_di();

/**
 * @brief 初始化RS485使能引脚
 * 
 */
void init_rs485_en();

/**
 * @brief 初始化按键引脚
 * 
 */
void init_key();

/**
 * @brief 处理按键中断
 * 
 */
void EXTI10_15_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_ADAPTER_H__ */
