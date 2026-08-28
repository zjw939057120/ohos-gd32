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

#include <stdarg.h>
#include <stdio.h>
#include "securec.h"
#include "uart.h"
#include "los_debug.h"
#include "los_interrupt.h"

static void dputs(char const *s)
{
    unsigned int intSave;

    intSave = LOS_IntLock();
	// 切换到发送模式
	rs485_3_en(true);
    while (*s) {
      usart_data_transmit(RS485_3_NUMBER, (uint8_t)*s);
      while(RESET == usart_flag_get(RS485_3_NUMBER, USART_FLAG_TBE));
      s++;
    }
	// 切换到接收模式
	rs485_3_en(false);
    LOS_IntRestore(intSave);
}

#ifdef LOSCFG_LIBC_NEWLIB
int __wrap_printf(char const *fmt, ...)
#else
int printf(const char *fmt, ...)
#endif
{
#define BUFSIZE  256
    char buf[BUFSIZE] = { 0 };
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf_s(buf, sizeof(buf), BUFSIZE - 1, fmt, ap);
    va_end(ap);
    if (len > 0) {
        dputs(buf);
    } else {
        dputs("printf error!\n");
    }
    return len;
}

int __wrap_sprintf(char *buffer, const char *format, ...)
{
    va_list va;
    va_start(va, format);
    const int ret = vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, format, va);
    va_end(va);
    return ret;
}

int __wrap_snprintf(char *buffer, int count, const char *format, ...)
{
    va_list va;
    va_start(va, format);
    const int ret = vsnprintf_s(buffer, count, count - 1, format, va);
    va_end(va);
    return ret;
}

int __wrap_vsnprintf(char* buffer, size_t count, const char* format, va_list va)
{
    return vsnprintf_s(buffer, count, count - 1, format, va);
}
