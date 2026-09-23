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

#ifndef _GPIO_ADAPTER_H
#define _GPIO_ADAPTER_H

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "los_event.h"
#include "los_compiler.h"
#include "pin_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

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
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _GPIO_ADAPTER_H */
