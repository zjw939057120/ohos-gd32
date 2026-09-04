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

#ifndef __SPI0_ADAPTER_H__
#define __SPI0_ADAPTER_H__

#include "gd32f4xx.h"
#include "gd32f4xx_misc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 计量模块 片选引脚 (PG15)
#define MEASURE_CS_LOW()    gpio_bit_reset(GPIOG, GPIO_PIN_15)
#define MEASURE_CS_HIGH()   gpio_bit_set(GPIOG, GPIO_PIN_15)

/**
 * @brief 初始化 计量模块 SPI 通信参数
 *
 * @param trans_mode 传输模式
 * @param device_mode 设备模式
 * @param frame_size 帧大小
 * @param clock_polarity_phase 时钟极性/相位
 * @param nss NSS 选择
 * @param prescale 时钟分频
 * @param endian 大端/小端
 */
void init_measure_spi_by_parameter(uint32_t trans_mode, uint32_t device_mode, uint32_t frame_size, uint32_t clock_polarity_phase, uint32_t nss, uint32_t prescale, uint32_t endian);

/**
 * @brief 初始化 计量模块 SPI 通信
 *
 */
void init_measure_spi(void);

/**
 * @brief 从 计量模块 读取数据
 *
 * @param data 数据
 * @return uint16_t 读取到的数据
 */
uint16_t measure_spi_read(uint16_t data);

#ifdef __cplusplus
}
#endif

#endif /* __SPI0_ADAPTER_H__ */
