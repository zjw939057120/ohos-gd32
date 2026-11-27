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

#include "uart.h"
#include "los_interrupt.h"
#include "los_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* USART TX/RX pins on GPIOA, alternate function 7 */
#define CONSOLE_UART                         USART0
#define CONSOLE_UART_RCU                     RCU_USART0
#define CONSOLE_UART_IRQ                     USART0_IRQn
#define CONSOLE_UART_BAUD                    (9600U)

#define CONSOLE_UART_GPIO_RCU                RCU_GPIOA
#define CONSOLE_UART_PORT                    GPIOA
#define CONSOLE_UART_TX_PIN                  GPIO_PIN_9
#define CONSOLE_UART_RX_PIN                  GPIO_PIN_10
#define CONSOLE_UART_AF                      GPIO_AF_7

/* g_shellInputEvent is defined by the kernel shell (components/shell/src/base/shmsg.c);
 * it is declared extern in uart.h and signalled from the USART0 RX interrupt. */

#if (LOSCFG_USE_SHELL == 1)
#define RX_BUF_SIZE                     128
static uint8_t rx_buf[RX_BUF_SIZE];
static uint16_t tx_index;
static uint16_t rx_index;

int32_t UartGetc(void)
{
    uint8_t c = 0;
    if (tx_index != rx_index) {
        c = rx_buf[tx_index++];
        tx_index %= RX_BUF_SIZE;     
    }
    return c;
}
#endif

int32_t UartPutc(int32_t c, void *file)
{
    (void)file;
    /* wait until the transmit data buffer is empty */
    while (RESET == usart_flag_get(CONSOLE_UART, USART_FLAG_TBE)) {
    }
    usart_data_transmit(CONSOLE_UART, (uint16_t)(c & 0xFF));
    return c;
}


void UartReceiveHandler(void)
{
    if(usart_flag_get(CONSOLE_UART, USART_FLAG_RBNE) != RESET)    //判断是否接收中断标志位置位
    {      
        usart_flag_clear(CONSOLE_UART, USART_FLAG_RBNE);
        uint8_t c = usart_data_receive(CONSOLE_UART);    //将接收到的数据存入buf    
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

void UartInit(void)
{
    /* enable GPIOA and USART0 clocks */
    rcu_periph_clock_enable(CONSOLE_UART_GPIO_RCU);
    rcu_periph_clock_enable(CONSOLE_UART_RCU);

    /* PA9 / PA10 -> AF7 (USART0_TX / USART0_RX) */
    gpio_af_set(CONSOLE_UART_PORT, CONSOLE_UART_AF, CONSOLE_UART_TX_PIN | CONSOLE_UART_RX_PIN);
    gpio_mode_set(CONSOLE_UART_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_UART_TX_PIN | CONSOLE_UART_RX_PIN);
    gpio_output_options_set(CONSOLE_UART_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CONSOLE_UART_TX_PIN | CONSOLE_UART_RX_PIN);

    /* USART0: 115200 8N1, default word length / stop bit */
    usart_deinit(CONSOLE_UART);
    usart_baudrate_set(CONSOLE_UART, CONSOLE_UART_BAUD);
    usart_receive_config(CONSOLE_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(CONSOLE_UART, USART_TRANSMIT_ENABLE);
    usart_enable(CONSOLE_UART);

    /* g_shellInputEvent is initialized by the shell (shmsg.c) during LosShellInit. */
}

void UartRxIrqRegister(void)
{
    /* NVIC + USART0 RBNE interrupt, then hook the LiteOS-M HWI */
    nvic_irq_enable(CONSOLE_UART_IRQ, 0, 0);
    usart_interrupt_enable(CONSOLE_UART, USART_INT_RBNE);
    (void)LOS_HwiCreate(CONSOLE_UART_IRQ, 0, 0, (HWI_PROC_FUNC)UartReceiveHandler, 0);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
