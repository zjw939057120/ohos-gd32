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


#include "rs485_adapter.h"
#include "los_interrupt.h"
#include "los_event.h"
#include "pin_config.h"
#include "queue_adapter.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void rs485_1_send(uint8_t *data, int len)
{
	int i;
    // 切换到发送模式
	rs485_1_en(true);
	for(i = 0; i < len; i++)
	{
        usart_data_transmit(RS485_1_NUMBER, (uint8_t)data[i]);
        while(RESET == usart_flag_get(RS485_1_NUMBER, USART_FLAG_TBE));
	}
	// 切换到接收模式
	rs485_1_en(false);
}

void rs485_2_send(uint8_t *data, int len)
{
	int i;
	// 切换到发送模式
	rs485_2_en(true);
	for(i = 0; i < len; i++)
	{
        usart_data_transmit(RS485_2_NUMBER, (uint8_t)data[i]);
        while(RESET == usart_flag_get(RS485_2_NUMBER, USART_FLAG_TBE));
	}
	// 切换到接收模式
	rs485_2_en(false);
}

void rs485_3_send(uint8_t *data, int len)
{
	int i;
	// 切换到发送模式
	rs485_3_en(true);
	for(i = 0; i < len; i++)
	{
        usart_data_transmit(RS485_3_NUMBER, (uint8_t)data[i]);
        while(RESET == usart_flag_get(RS485_3_NUMBER, USART_FLAG_TBE));
	}
	// 切换到接收模式
	rs485_3_en(false);
}

uint8_t rs485_1_rx_buf[MAX_MSG_SIZE];/* 接收缓冲区 */
uint16_t rs485_1_rx_index = 0;/* 接收缓冲区索引 */
uint8_t rs485_1_rx_temp = 0;/* 接收缓冲区临时变量 */

void RS485_1_IRQHandler(void)
{
    if(usart_flag_get(RS485_1_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_1_NUMBER, USART_FLAG_RBNE);
        rs485_1_rx_temp = usart_data_receive(RS485_1_NUMBER);
        if(rs485_1_rx_index < MAX_MSG_SIZE){
            rs485_1_rx_buf[rs485_1_rx_index++] = rs485_1_rx_temp;    //将接收到的数据存入buf    
        }else{
            // 接收缓冲区已满，丢弃数据
            rs485_1_rx_index = 0;
        }
    }
    else if(usart_flag_get(RS485_1_NUMBER, USART_FLAG_IDLE) != RESET)
    {
        // 清空空闲中断标志位
        usart_flag_get(RS485_1_NUMBER, USART_FLAG_IDLE);
        usart_data_receive(RS485_1_NUMBER);
        UINT32 ret = rs485_1_mq_send(rs485_1_rx_buf, rs485_1_rx_index);
        // 清空接收缓冲区
        rs485_1_rx_index = 0;
        if(ret != LOS_OK)
        {
            printf("rs485_1_mq_send fail! ret=%d\n", ret);
        }
    }
}

uint8_t rs485_2_rx_buf[MAX_MSG_SIZE];/* 接收缓冲区 */
uint16_t rs485_2_rx_index = 0;/* 接收缓冲区索引 */
uint8_t rs485_2_rx_temp = 0;/* 接收缓冲区临时变量 */

void RS485_2_IRQHandler(void)
{
    if(usart_flag_get(RS485_2_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_2_NUMBER, USART_FLAG_RBNE);
        rs485_2_rx_temp = usart_data_receive(RS485_2_NUMBER);
        if(rs485_2_rx_index < MAX_MSG_SIZE){
            rs485_2_rx_buf[rs485_2_rx_index++] = rs485_2_rx_temp;    //将接收到的数据存入buf    
        }else{
            // 接收缓冲区已满，丢弃数据
            rs485_2_rx_index = 0;
        }
    }
    else if(usart_flag_get(RS485_2_NUMBER, USART_FLAG_IDLE) != RESET)
    {
        // 清空空闲中断标志位
        usart_flag_get(RS485_2_NUMBER, USART_FLAG_IDLE);
        usart_data_receive(RS485_2_NUMBER);
        UINT32 ret = rs485_2_mq_send(rs485_2_rx_buf, rs485_2_rx_index);
        // 清空接收缓冲区
        rs485_2_rx_index = 0;
        if(ret != LOS_OK)
        {
            printf("rs485_2_mq_send fail! ret=%d\n", ret);
        }
    }
}

uint8_t rs485_3_rx_buf[MAX_MSG_SIZE];/* 接收缓冲区 */
uint16_t rs485_3_rx_index = 0;/* 接收缓冲区索引 */
uint8_t rs485_3_rx_temp = 0;/* 接收缓冲区临时变量 */

void RS485_3_IRQHandler(void)
{
    if(usart_flag_get(RS485_3_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_3_NUMBER, USART_FLAG_RBNE);
        rs485_3_rx_temp = usart_data_receive(RS485_3_NUMBER);
        if(rs485_3_rx_index < MAX_MSG_SIZE){
            rs485_3_rx_buf[rs485_3_rx_index++] = rs485_3_rx_temp;    //将接收到的数据存入buf    
        }else{
            // 接收缓冲区已满，丢弃数据
            rs485_3_rx_index = 0;
        }
    }
    else if(usart_flag_get(RS485_3_NUMBER, USART_FLAG_IDLE) != RESET)
    {
        // 清空空闲中断标志位
        usart_flag_get(RS485_3_NUMBER, USART_FLAG_IDLE);
        usart_data_receive(RS485_3_NUMBER);
        UINT32 ret = rs485_3_mq_send(rs485_3_rx_buf, rs485_3_rx_index);
        // 清空接收缓冲区
        rs485_3_rx_index = 0;
        if(ret != LOS_OK)
        {
            printf("rs485_3_mq_send fail! ret=%d\n", ret);
        }
    }
}

void rs485_1_irq_register(void)
{
    /* USART 中断配置 */
    nvic_irq_enable(RS485_1_IRQ, 1, 0);
    /* 使能串口接收中断 */
	usart_interrupt_enable(RS485_1_NUMBER, USART_INT_RBNE);
    /* 使能空闲中断 */
    usart_interrupt_enable(RS485_1_NUMBER, USART_INT_IDLE);

    int ret = LOS_HwiCreate(RS485_1_IRQ, 0, 0, (HWI_PROC_FUNC)RS485_1_IRQHandler, NULL);  /* 中断号 优先级 中断模式 处理函数 传入参数 */
    if (ret != LOS_OK) {
            printf("%s:%d Interrupt Create fail! ret=%x  RS485_1_IRQ=%d\n", __FILE__, __LINE__, ret, RS485_1_IRQ);
    }
}

void rs485_2_irq_register(void)
{
    /* USART 中断配置 */
    nvic_irq_enable(RS485_2_IRQ, 1, 0);
    /* 使能串口接收中断 */
	usart_interrupt_enable(RS485_2_NUMBER, USART_INT_RBNE);
    /* 使能空闲中断 */
    usart_interrupt_enable(RS485_2_NUMBER, USART_INT_IDLE);

    int ret = LOS_HwiCreate(RS485_2_IRQ, 0, 0, (HWI_PROC_FUNC)RS485_2_IRQHandler, NULL);  /* 中断号 优先级 中断模式 处理函数 传入参数 */
    if (ret != LOS_OK) {
            printf("%s:%d Interrupt Create fail! ret=%x  RS485_2_IRQ=%d\n", __FILE__, __LINE__, ret, RS485_2_IRQ);
    }
}

void rs485_3_irq_register(void)
{
    /* USART 中断配置 */
    nvic_irq_enable(RS485_3_IRQ, 1, 0);
    /* 使能串口接收中断 */
	usart_interrupt_enable(RS485_3_NUMBER, USART_INT_RBNE);
    /* 使能空闲中断 */
    usart_interrupt_enable(RS485_3_NUMBER, USART_INT_IDLE);

    int ret = LOS_HwiCreate(RS485_3_IRQ, 0, 0, (HWI_PROC_FUNC)RS485_3_IRQHandler, NULL);  /* 中断号 优先级 中断模式 处理函数 传入参数 */
    if (ret != LOS_OK) {
            printf("%s:%d Interrupt Create fail! ret=%x  RS485_3_IRQ=%d\n", __FILE__, __LINE__, ret, RS485_3_IRQ);
    }
}

void rs485_irq_register(void)
{
    // 注册RS485_1中断
    rs485_1_irq_register();
    // 注册RS485_2中断
    rs485_2_irq_register();
#if (LOSCFG_USE_SHELL != 1)
    // 注册RS485_3中断
    rs485_3_irq_register();
#endif
}

void init_rs485_1(void)
{
	//使能 GPIO 时钟源
    rcu_periph_clock_enable(RS485_1_GPIO_RXD_RCU);
    rcu_periph_clock_enable(RS485_1_GPIO_TXD_RCU);

    //使能 USART 时钟源
    rcu_periph_clock_enable(RS485_1_RCU);

    /* 复用引脚为 USARTx_Tx */
    gpio_af_set(RS485_1_GPIO_RXD_PORT, RS485_1_GPIO_RXD_AF, RS485_1_GPIO_TXD_PIN);

    /* 复用引脚为 USARTx_Rx */
    gpio_af_set(RS485_1_GPIO_TXD_PORT, RS485_1_GPIO_TXD_AF, RS485_1_GPIO_RXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(RS485_1_GPIO_TXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_1_GPIO_TXD_PIN);
    gpio_output_options_set(RS485_1_GPIO_TXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_1_GPIO_TXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(RS485_1_GPIO_RXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_1_GPIO_RXD_PIN);
    gpio_output_options_set(RS485_1_GPIO_RXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_1_GPIO_RXD_PIN);

    /* USART 配置 */
    usart_deinit(RS485_1_NUMBER);
    usart_baudrate_set(RS485_1_NUMBER, RS485_1_BAUD);
    usart_receive_config(RS485_1_NUMBER, USART_RECEIVE_ENABLE);
    usart_transmit_config(RS485_1_NUMBER, USART_TRANSMIT_ENABLE);
    usart_enable(RS485_1_NUMBER);
}

void init_rs485_2(void)
{
	//使能 GPIO 时钟源
    rcu_periph_clock_enable(RS485_2_GPIO_RXD_RCU);
    rcu_periph_clock_enable(RS485_2_GPIO_TXD_RCU);

    //使能 USART 时钟源
    rcu_periph_clock_enable(RS485_2_RCU);

    /* 复用引脚为 USARTx_Tx */
    gpio_af_set(RS485_2_GPIO_RXD_PORT, RS485_2_GPIO_RXD_AF, RS485_2_GPIO_TXD_PIN);

    /* 复用引脚为 USARTx_Rx */
    gpio_af_set(RS485_2_GPIO_TXD_PORT, RS485_2_GPIO_TXD_AF, RS485_2_GPIO_RXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(RS485_2_GPIO_TXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_2_GPIO_TXD_PIN);
    gpio_output_options_set(RS485_2_GPIO_TXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_2_GPIO_TXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(RS485_2_GPIO_RXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_2_GPIO_RXD_PIN);
    gpio_output_options_set(RS485_2_GPIO_RXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_2_GPIO_RXD_PIN);

    /* USART 配置 */
    usart_deinit(RS485_2_NUMBER);
    usart_baudrate_set(RS485_2_NUMBER, RS485_2_BAUD);
    usart_receive_config(RS485_2_NUMBER, USART_RECEIVE_ENABLE);
    usart_transmit_config(RS485_2_NUMBER, USART_TRANSMIT_ENABLE);
    usart_enable(RS485_2_NUMBER);
}

void init_rs485_3(void)
{
	//使能 GPIO 时钟源
    rcu_periph_clock_enable(RS485_3_GPIO_RXD_RCU);
    rcu_periph_clock_enable(RS485_3_GPIO_TXD_RCU);

    //使能 USART 时钟源
    rcu_periph_clock_enable(RS485_3_RCU);

    /* 复用引脚为 USARTx_Tx */
    gpio_af_set(RS485_3_GPIO_RXD_PORT, RS485_3_GPIO_RXD_AF, RS485_3_GPIO_TXD_PIN);

    /* 复用引脚为 USARTx_Rx */
    gpio_af_set(RS485_3_GPIO_TXD_PORT, RS485_3_GPIO_TXD_AF, RS485_3_GPIO_RXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(RS485_3_GPIO_TXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_3_GPIO_TXD_PIN);
    gpio_output_options_set(RS485_3_GPIO_TXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_3_GPIO_TXD_PIN);

    /* 设置引脚为上拉复用 */
    gpio_mode_set(RS485_3_GPIO_RXD_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_3_GPIO_RXD_PIN);
    gpio_output_options_set(RS485_3_GPIO_RXD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_3_GPIO_RXD_PIN);

    /* USART 配置 */
    usart_deinit(RS485_3_NUMBER);
    usart_baudrate_set(RS485_3_NUMBER, RS485_3_BAUD);
    usart_receive_config(RS485_3_NUMBER, USART_RECEIVE_ENABLE);
    usart_transmit_config(RS485_3_NUMBER, USART_TRANSMIT_ENABLE);
    usart_enable(RS485_3_NUMBER);
}

void init_rs485(void)
{
    // 初始化RS485_1
    init_rs485_1();
    // 初始化RS485_2
    init_rs485_2();
#if (LOSCFG_USE_SHELL != 1)
    // 初始化RS485_3
    init_rs485_3();
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
