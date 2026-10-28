/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hal_sys_param.h"

#include <stdint.h>

// GD32F470 唯一设备ID的基地址
#ifndef GD32_UNIQUE_ID_BASE
#define GD32_UNIQUE_ID_BASE 0x1FFF7A10
#endif

/* 96 位 UID = 12 字节，转成十六进制字符串需要 24 个字符 + 1 个 '\0' */
#define UID_SERIAL_LEN 24

static char g_uid_serial[UID_SERIAL_LEN + 1] = {0};

static void uid_to_serial(void)
{
    const uint8_t *uid_ptr = (const uint8_t *)GD32_UNIQUE_ID_BASE;

    /* 把 12 字节 UID 转成 24 位十六进制字符串 */
    for (int i = 0; i < 12; i++) {
        uint8_t high = (uid_ptr[i] >> 4) & 0x0F;
        uint8_t low  = uid_ptr[i] & 0x0F;

        g_uid_serial[i * 2]     = (high < 10) ? ('0' + high) : ('A' + high - 10);
        g_uid_serial[i * 2 + 1] = (low  < 10) ? ('0' + low)  : ('A' + low  - 10);
    }

    g_uid_serial[UID_SERIAL_LEN] = '\0';
}

const char *HalGetSerial(void)
{
    static int inited = 0;

    if (!inited) {
        uid_to_serial();
        inited = 1;
    }

    return g_uid_serial;
}