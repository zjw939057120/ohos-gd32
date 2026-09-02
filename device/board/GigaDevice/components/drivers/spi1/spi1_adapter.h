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

#ifndef __SPI1_ADAPTER_H__
#define __SPI1_ADAPTER_H__

#include "gd32f4xx.h"
#include "gd32f4xx_misc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// TLC2543 片选引脚 (PF11)
#define TLC2543_CS_LOW()    gpio_bit_reset(GPIOF, GPIO_PIN_11)
#define TLC2543_CS_HIGH()   gpio_bit_set(GPIOF, GPIO_PIN_11)
// AD5318 片选引脚 (PB2)
#define AD5318_CS_LOW()   gpio_bit_reset(GPIOB, GPIO_PIN_2)
#define AD5318_CS_HIGH()  gpio_bit_set(GPIOB, GPIO_PIN_2)

/**
 * @brief 初始化 TLC2543 SPI 通信
 *
 */
void init_tlc2543_spi(void);

/**
 * @brief 从 TLC2543 读取 ADC 数据
 *
 * @param chan ADC 通道
 * @return uint16_t 读取到的 ADC 数据
 */
uint16_t tlc2543_read_adc(uint8_t chan);

/**
 * @brief 初始化 AD5318 SPI 通信
 *
 */
void init_ad5318_spi(void);

/**
 * @brief 向 AD5318 写入 DAC 数据
 *
 * @param channel DAC 通道
 * @param data DAC 数据
 */
void ad5318_write_dac(uint8_t channel, uint16_t data);

#ifdef __cplusplus
}
#endif

#endif /* __SPI1_ADAPTER_H__ */
