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

#ifndef __UART_H
#define __UART_H

#include "los_event.h"
#include "los_compiler.h"
#include "board.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern void UartInit(void);
extern int32_t UartPutc(int32_t c, void *file);
extern int32_t UartGetc(void);
extern void UartRxIrqRegister(void);

extern EVENT_CB_S g_shellInputEvent;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* _UART_H */
