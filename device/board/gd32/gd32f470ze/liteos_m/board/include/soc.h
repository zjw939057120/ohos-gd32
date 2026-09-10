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

#ifndef _SOC_H
#define _SOC_H

#include "gd32f4xx.h"

/* GD32F470ZE system clock: 240MHz from 8MHz HXTAL
 * (system_clock_240m_8m_hxtal in system_gd32f4xx.c) */
#define SYSCLK_FREQ        240000000

/* Debug console on USART0 (PA9 TX / PA10 RX) */
#define UART0_BAUDRATE     115200
#define Uart0_Rx_IRQn      USART0_IRQn

#endif /* _SOC_H */
