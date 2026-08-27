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

#include "uart.h"

void rs485_1_send_data(uint8_t *data, int len)
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

void rs485_2_send_data(uint8_t *data, int len)
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

void rs485_3_send_data(uint8_t *data, int len)
{
	int i;
	// 切换到接收模式
	rs485_3_en(false);
	for(i = 0; i < len; i++)
	{
        usart_data_transmit(RS485_3_NUMBER, (uint8_t)data[i]);
        while(RESET == usart_flag_get(RS485_3_NUMBER, USART_FLAG_TBE));
	}
	// 切换到接收模式
	rs485_3_en(false);
}

void rs485_1_send_string(uint8_t *data)
{
    rs485_1_send_data(data, strlen(data));
}

void rs485_2_send_string(uint8_t *data)
{
    rs485_2_send_data(data, strlen(data));
}

void rs485_3_send_string(uint8_t *data)
{
    rs485_3_send_data(data, strlen(data));
}



#if (LOSCFG_USE_SHELL == 1)
#define RX_BUF_SIZE                     128
static uint8_t rx_buf[RX_BUF_SIZE];
static uint16_t tx_index;
static uint16_t rx_index;

INT32 UartGetc(VOID)
{
    uint8_t c = 0;
    if (tx_index != rx_index) {
        c = rx_buf[tx_index++];
        tx_index %= RX_BUF_SIZE;     
    }
    return c;
}
#endif

void RS485_1_IRQHandler(void)
{
    if(usart_flag_get(RS485_1_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_1_NUMBER, USART_FLAG_RBNE);
        uint8_t c = usart_data_receive(RS485_1_NUMBER);    //将接收到的数据存入buf    
    }
    else if(usart_flag_get(RS485_1_NUMBER, USART_FLAG_IDLE) != RESET)
    {
        // 清空空闲中断标志位
        usart_flag_get(RS485_1_NUMBER, USART_FLAG_IDLE);
        usart_data_receive(RS485_1_NUMBER);
        rs485_1_send_string("IDLE\n");
    }
}

void RS485_2_IRQHandler(void)
{
    if(usart_flag_get(RS485_2_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_2_NUMBER, USART_FLAG_RBNE);
        uint8_t c = usart_data_receive(RS485_2_NUMBER);    //将接收到的数据存入buf
    }
    else if(usart_flag_get(RS485_2_NUMBER, USART_FLAG_IDLE) != RESET)
    {
        // 清空空闲中断标志位
        usart_flag_get(RS485_2_NUMBER, USART_FLAG_IDLE);
        usart_data_receive(RS485_2_NUMBER);
        rs485_2_send_string("IDLE\n");
    }
}

void RS485_3_IRQHandler(void)
{
    if(usart_flag_get(RS485_3_NUMBER, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_3_NUMBER, USART_FLAG_RBNE);
        uint8_t c = usart_data_receive(RS485_3_NUMBER);    //将接收到的数据存入buf    
#if (LOSCFG_USE_SHELL == 1)
        rx_buf[rx_index++] = c;
        rx_index %= RX_BUF_SIZE;
        if (rx_index == tx_index) {
            tx_index++;
            tx_index %= RX_BUF_SIZE;
        }
        (void)LOS_EventWrite(&g_shellInputEvent, 0x1);
#endif
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

#if (LOSCFG_USE_SHELL == 1)
    int ret = LOS_HwiCreate(RS485_3_IRQ, 0, 0, (HWI_PROC_FUNC)RS485_3_IRQHandler, NULL);  /* 中断号 优先级 中断模式 处理函数 传入参数 */
    if (ret != LOS_OK) {
            printf("%s:%d Interrupt Create fail! ret=%x  RS485_3_IRQ=%d\n", __FILE__, __LINE__, ret, RS485_3_IRQ);
    }
#endif
}

void uart_irq_register(void)
{
    //注册RS485中断
    rs485_1_irq_register();
    rs485_2_irq_register();
    rs485_3_irq_register();
}

void rs485_1_init(void)
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

void rs485_2_init(void)
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

void rs485_3_init(void)
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

void uart_init(void)
{
    //初始化RS485
    rs485_1_init();
    rs485_2_init();
    rs485_3_init();
}
