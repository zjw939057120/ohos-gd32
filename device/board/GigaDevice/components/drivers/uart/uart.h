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

#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include "gd32f4xx.h"
#include "gd32f4xx_usart.h"

#include "los_compiler.h"
#include "los_event.h"
#include "los_interrupt.h"
#include "gpio_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif
// RS485_1 USART0 PB6,PB7,AP7
#define RS485_1_NUMBER                          USART0
#define RS485_1_RCU                             RCU_USART0
#define RS485_1_IRQ                             USART0_IRQn
#define RS485_1_BAUD                            (9600U)

#define RS485_1_GPIO_TXD_RCU                    RCU_GPIOB
#define RS485_1_GPIO_TXD_PORT                   GPIOB
#define RS485_1_GPIO_TXD_PIN                    GPIO_PIN_6
#define RS485_1_GPIO_TXD_AF                     GPIO_AF_7

#define RS485_1_GPIO_RXD_RCU                    RCU_GPIOB
#define RS485_1_GPIO_RXD_PORT                   GPIOB
#define RS485_1_GPIO_RXD_PIN                    GPIO_PIN_7
#define RS485_1_GPIO_RXD_AF                     GPIO_AF_7

// RS485_2 USART2 PB10,PB11,AP7
#define RS485_2_NUMBER                          USART2
#define RS485_2_RCU                             RCU_USART2
#define RS485_2_IRQ                             USART2_IRQn
#define RS485_2_BAUD                            (9600U)

#define RS485_2_GPIO_TXD_RCU                    RCU_GPIOB
#define RS485_2_GPIO_TXD_PORT                   GPIOB
#define RS485_2_GPIO_TXD_PIN                    GPIO_PIN_10
#define RS485_2_GPIO_TXD_AF                     GPIO_AF_7

#define RS485_2_GPIO_RXD_RCU                    RCU_GPIOB
#define RS485_2_GPIO_RXD_PORT                   GPIOB
#define RS485_2_GPIO_RXD_PIN                    GPIO_PIN_11
#define RS485_2_GPIO_RXD_AF                     GPIO_AF_7

// RS485_3 USART5 PC6,PC7,AF8
#define RS485_3_NUMBER                          USART5
#define RS485_3_RCU                             RCU_USART5
#define RS485_3_IRQ                             USART5_IRQn
#define RS485_3_BAUD                            (9600U)

#define RS485_3_GPIO_TXD_RCU                    RCU_GPIOC
#define RS485_3_GPIO_TXD_PORT                   GPIOC
#define RS485_3_GPIO_TXD_PIN                    GPIO_PIN_6
#define RS485_3_GPIO_TXD_AF                     GPIO_AF_8

#define RS485_3_GPIO_RXD_RCU                    RCU_GPIOC
#define RS485_3_GPIO_RXD_PORT                   GPIOC
#define RS485_3_GPIO_RXD_PIN                    GPIO_PIN_7
#define RS485_3_GPIO_RXD_AF                     GPIO_AF_8

// RS485_1_LED PA10
#define RS485_1_LED_GPIO_RCU        RCU_GPIOA
#define RS485_1_LED_GPIO_PORT		GPIOA
#define RS485_1_LED_PIN				GPIO_PIN_10
// RS485_2_LED PC8
#define RS485_2_LED_GPIO_RCU        RCU_GPIOC
#define RS485_2_LED_GPIO_PORT		GPIOC
#define RS485_2_LED_PIN				GPIO_PIN_8
// RS485_3_LED PA11
#define RS485_3_LED_GPIO_RCU        RCU_GPIOA
#define RS485_3_LED_GPIO_PORT		GPIOA
#define RS485_3_LED_PIN				GPIO_PIN_11
// RS485_1_EN PB9
#define RS485_1_EN_GPIO_RCU         RCU_GPIOB
#define RS485_1_EN_GPIO_PORT        GPIOB
#define RS485_1_EN_GPIO_PIN			GPIO_PIN_9
// RS485_2_EN PB8
#define RS485_2_EN_GPIO_RCU         RCU_GPIOB
#define RS485_2_EN_GPIO_PORT        GPIOB
#define RS485_2_EN_GPIO_PIN			GPIO_PIN_8
// RS485_3_EN PD7
#define RS485_3_EN_GPIO_RCU         RCU_GPIOD
#define RS485_3_EN_GPIO_PORT        GPIOD
#define RS485_3_EN_GPIO_PIN			GPIO_PIN_7


void uart_init(void);
void uart_irq_register(void);

void rs485_1_send_data(uint8_t *data, int len);
void rs485_2_send_data(uint8_t *data, int len);
void rs485_3_send_data(uint8_t *data, int len);

void rs485_1_send_string(uint8_t *data);
void rs485_2_send_string(uint8_t *data);
void rs485_3_send_string(uint8_t *data);

extern INT32 UartGetc(VOID);
extern EVENT_CB_S g_shellInputEvent;

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
