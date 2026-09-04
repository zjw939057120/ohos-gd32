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

#include "spi0_adapter.h"


static spi_parameter_struct spi_init_struct = 
{
    .trans_mode = SPI_TRANSMODE_FULLDUPLEX,                        
    .device_mode = SPI_MASTER,
    .frame_size = SPI_FRAMESIZE_16BIT,
    .clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE,
    .nss = SPI_NSS_SOFT,
    .prescale = SPI_PSC_32,
    .endian = SPI_ENDIAN_MSB
};

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
void init_measure_spi_by_parameter(uint32_t trans_mode, uint32_t device_mode, uint32_t frame_size, uint32_t clock_polarity_phase, uint32_t nss, uint32_t prescale, uint32_t endian)
{
    spi_init_struct.trans_mode = trans_mode; /* 传输模式 */
    spi_init_struct.device_mode = device_mode; /* 设备模式 */
    spi_init_struct.frame_size = frame_size; /* 帧大小 */
    spi_init_struct.clock_polarity_phase = clock_polarity_phase; /* 时钟极性/相位 */
    spi_init_struct.nss = nss; /* NSS 选择 */
    spi_init_struct.prescale = prescale; /* 时钟分频 */
    spi_init_struct.endian = endian; /* 大端/小端 */
    init_measure_spi();
}

/**
 * @brief 初始化 计量模块 SPI 通信
 *
 */
void init_measure_spi(void)
{

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI0);

    /* SPI0_CLK(PB3), SPI0_MISO(PB4), SPI0_MOSI(PB5) GPIO pin configuration */
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_3|GPIO_PIN_4| GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3|GPIO_PIN_4| GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_3|GPIO_PIN_4| GPIO_PIN_5);

    /* SPI0_CS(PG15) GPIO pin configuration */
    rcu_periph_clock_enable(RCU_GPIOG);
    gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    /* chip select invalid */
    MEASURE_CS_HIGH();

    /* SPI0 parameter config */
    // spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    // spi_init_struct.device_mode          = SPI_MASTER;
    // spi_init_struct.frame_size           = SPI_FRAMESIZE_16BIT;
    // spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    // spi_init_struct.nss                  = SPI_NSS_SOFT;
    // spi_init_struct.prescale             = SPI_PSC_32;
    // spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    /* enable SPI0 */
    spi_enable(SPI0);
}

/**
 * @brief 读取 计量模块 数据
 * @param data 数据
 * @return uint16_t 读取到的数据
 */
uint16_t measure_spi_read(uint16_t data)
{
    uint16_t control_word = 0;
    uint16_t received_data = 0;

    // 1. 拉低片选
    MEASURE_CS_LOW();

    // 2. 发送控制字，同时接收上一次转换的结果
    spi_i2s_data_transmit(SPI0, data);
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));     // 等待发送缓冲区空
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));    // 等待接收缓冲区非空
    received_data = spi_i2s_data_receive(SPI0);                    // 读取数据

    // 3. 拉高片选，结束本次通讯
    MEASURE_CS_HIGH();

    // 4. 返回数据
    return received_data;
}
