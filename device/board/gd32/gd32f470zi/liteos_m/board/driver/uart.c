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
 * Debug console UART driver for GD32F470ZI.
 * USART0 on PA9 (TX) / PA10 (RX), AF7, 115200 8N1.
 * Output goes through UartPutc (used by libc/dprintf.c printf); shell input
 * arrives via the USART0 RBNE interrupt which signals g_shellInputEvent.
 */

#include "uart.h"
#include "soc.h"
#include "los_interrupt.h"
#include "los_event.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* USART0 TX/RX pins on GPIOA, alternate function 7 */
#define UART0_TX_PIN        GPIO_PIN_9
#define UART0_RX_PIN        GPIO_PIN_10
#define UART0_GPIO_AF       GPIO_AF_7

/* g_shellInputEvent is defined by the kernel shell (components/shell/src/base/shmsg.c);
 * it is declared extern in uart.h and signalled from the USART0 RX interrupt. */

INT32 UartGetc(VOID)
{
    if (RESET == usart_flag_get(USART0, USART_FLAG_RBNE)) {
        return 0;
    }
    return (INT32)(usart_data_receive(USART0) & 0xFF);
}

INT32 UartPutc(INT32 c, VOID *file)
{
    (void)file;
    /* wait until the transmit data buffer is empty */
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE)) {
    }
    usart_data_transmit(USART0, (uint16_t)(c & 0xFF));
    return c;
}

VOID UartReceiveHandler(VOID)
{
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) {
        (void)LOS_EventWrite(&g_shellInputEvent, 0x1);
    }
}

VOID UartInit(VOID)
{
    /* enable GPIOA and USART0 clocks */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);

    /* PA9 / PA10 -> AF7 (USART0_TX / USART0_RX) */
    gpio_af_set(GPIOA, UART0_GPIO_AF, UART0_TX_PIN | UART0_RX_PIN);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, UART0_TX_PIN | UART0_RX_PIN);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, UART0_TX_PIN | UART0_RX_PIN);

    /* USART0: 115200 8N1, default word length / stop bit */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, UART0_BAUDRATE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);

    /* g_shellInputEvent is initialized by the shell (shmsg.c) during LosShellInit. */
}

VOID Uart0RxIrqRegister(VOID)
{
    /* NVIC + USART0 RBNE interrupt, then hook the LiteOS-M HWI */
    nvic_irq_enable(USART0_IRQn, 0, 0);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    (void)LOS_HwiCreate(USART0_IRQn, 0, 0, (HWI_PROC_FUNC)UartReceiveHandler, 0);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
