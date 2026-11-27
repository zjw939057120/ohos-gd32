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

/*
 * Debug console UART driver for GD32F470VG.
 * USART0 on PA9 (TX) / PA10 (RX), AF7, 115200 8N1.
 * Output goes through UartPutc (used by libc/dprintf.c printf); shell input
 * arrives via the USART0 RBNE interrupt which signals g_shellInputEvent.
 */

#include "rs485.h"
#include "los_interrupt.h"
#include "los_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* RS485 TX/RX pins on GPIOA, alternate function 7 */
#define RS485_UART                         USART0
#define RS485_UART_RCU                     RCU_USART0
#define RS485_UART_IRQ                     USART0_IRQn
#define RS485_UART_BAUD                    (9600U)

#define RS485_UART_GPIO_RCU                RCU_GPIOA
#define RS485_UART_PORT                    GPIOA
#define RS485_UART_TX_PIN                  GPIO_PIN_9
#define RS485_UART_RX_PIN                  GPIO_PIN_10
#define RS485_UART_AF                      GPIO_AF_7

#define RS485_EN_RCU                       RCU_GPIOA
#define RS485_EN_PORT                      GPIOA
#define RS485_EN_PIN                       GPIO_PIN_11

uint32_t Rs485Puts(uint8_t *str)
{
    Rs485En(true);
    while (*str) {
        usart_data_transmit(RS485_UART, (uint16_t)*str++);
        while(RESET == usart_flag_get(RS485_UART, USART_FLAG_TBE));
    }
    Rs485En(false);
    return str;
}


void Rs485ReceiveHandler(void)
{
    if(usart_flag_get(RS485_UART, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(RS485_UART, USART_FLAG_RBNE);
        uint8_t c = usart_data_receive(RS485_UART);    //将接收到的数据存入buf    
    }
}

void Rs485Init(void)
{
    /* enable GPIOA and USART0 clocks */
    rcu_periph_clock_enable(RS485_UART_GPIO_RCU);
    rcu_periph_clock_enable(RS485_UART_RCU);

    /* PA9 / PA10 -> AF7 (USART0_TX / USART0_RX) */
    gpio_af_set(RS485_UART_PORT, RS485_UART_AF, RS485_UART_TX_PIN | RS485_UART_RX_PIN);
    gpio_mode_set(RS485_UART_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, RS485_UART_TX_PIN | RS485_UART_RX_PIN);
    gpio_output_options_set(RS485_UART_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_UART_TX_PIN | RS485_UART_RX_PIN);

    /* USART0: 115200 8N1, default word length / stop bit */
    usart_deinit(RS485_UART);
    usart_baudrate_set(RS485_UART, RS485_UART_BAUD);
    usart_receive_config(RS485_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(RS485_UART, USART_TRANSMIT_ENABLE);
    usart_enable(RS485_UART);

}

void Rs485RxIrqRegister(void)
{
    /* NVIC + USART0 RBNE interrupt, then hook the LiteOS-M HWI */
    nvic_irq_enable(RS485_UART_IRQ, 0, 0);
    usart_interrupt_enable(RS485_UART, USART_INT_RBNE);
    (void)LOS_HwiCreate(RS485_UART_IRQ, 0, 0, (HWI_PROC_FUNC)Rs485ReceiveHandler, 0);
}

void Rs485EnInit(void)
{
    rcu_periph_clock_enable(RS485_EN_RCU);

    gpio_mode_set(RS485_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, RS485_EN_PIN);
    gpio_output_options_set(RS485_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RS485_EN_PIN);

    gpio_bit_reset(RS485_EN_PORT, RS485_EN_PIN);
}

void Rs485En(bool set)
{
    set == true ? gpio_bit_set(RS485_EN_PORT, RS485_EN_PIN) : gpio_bit_reset(RS485_EN_PORT, RS485_EN_PIN);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
