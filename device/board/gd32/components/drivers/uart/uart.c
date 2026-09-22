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
 * Debug console UART driver for GD32F470ZG.
 * USART0 on PA9 (TX) / PA10 (RX), AF7, 115200 8N1.
 * Output goes through UartPutc (used by libc/dprintf.c printf); shell input
 * arrives via the USART0 RBNE interrupt which signals g_shellInputEvent.
 */

#include "uart.h"
#include "los_interrupt.h"
#include "los_event.h"
#include "pin_config.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* USART5 TX/RX pins on GPIOC, alternate function 8 */
#define UART_TX_PIN        GPIO_PIN_6
#define UART_RX_PIN        GPIO_PIN_7
#define UART_GPIO_AF       GPIO_AF_8

/* g_shellInputEvent is defined by the kernel shell (components/shell/src/base/shmsg.c);
 * it is declared extern in uart.h and signalled from the USART0 RX interrupt. */

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

INT32 UartPutc(INT32 c, VOID *file)
{
    (void)file;
    /* wait until the transmit data buffer is empty */
    while (RESET == usart_flag_get(USART5, USART_FLAG_TBE)) {
    }
    usart_data_transmit(USART5, (uint16_t)(c & 0xFF));
    return c;
}

VOID UartReceiveHandler(VOID)
{
    if (RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_RBNE)) {
        uint8_t c = usart_data_receive(USART5); 
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

VOID UartInit(VOID)
{
    /* enable GPIOC and USART5 clocks */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_USART5);

    /* GPIOC6 / GPIOC7 -> AF8 (USART5_TX / USART5_RX) */
    gpio_af_set(GPIOC, UART_GPIO_AF, UART_TX_PIN | UART_RX_PIN);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART_TX_PIN | UART_RX_PIN);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART_TX_PIN | UART_RX_PIN);

    /* USART5: 115200 8N1, default word length / stop bit */
    usart_deinit(USART5);
    usart_baudrate_set(USART5, 115200);
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);
    usart_enable(USART5);

    /* g_shellInputEvent is initialized by the shell (shmsg.c) during LosShellInit. */
}

VOID UartRxIrqRegister(VOID)
{
    /* NVIC + USART5 RBNE interrupt, then hook the LiteOS-M HWI */
    nvic_irq_enable(USART5_IRQn, 0, 0);
    usart_interrupt_enable(USART5, USART_INT_RBNE);
    (void)LOS_HwiCreate(USART5_IRQn, 0, 0, (HWI_PROC_FUNC)UartReceiveHandler, 0);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
