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
 * Stub file-system HAL for GD32F470ZI.
 *
 * This board has no on-board file system (storage_type is empty in the product
 * config), so there is nowhere to persist data. The OHOS parameter service
 * (LiteParamService, pulled in by OHOS_SystemInit) still calls the UtilsFile*
 * API via these HalFile* entry points. Returning -1 makes every operation fail
 * harmlessly: the param service simply finds no persistent parameter file and
 * continues with in-memory defaults.
 */

#include "hal_file.h"
#include <stddef.h>

int HalFileOpen(const char *path, int oflag, int mode)
{
    (void)path;
    (void)oflag;
    (void)mode;
    return -1;
}

int HalFileClose(int fd)
{
    (void)fd;
    return -1;
}

int HalFileRead(int fd, char *buf, unsigned int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    return -1;
}

int HalFileWrite(int fd, const char *buf, unsigned int len)
{
    (void)fd;
    (void)buf;
    (void)len;
    return -1;
}

int HalFileDelete(const char *path)
{
    (void)path;
    return -1;
}

int HalFileStat(const char *path, unsigned int *fileSize)
{
    (void)path;
    if (fileSize != NULL) {
        *fileSize = 0;
    }
    return -1;
}

int HalFileSeek(int fd, int offset, unsigned int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return -1;
}
