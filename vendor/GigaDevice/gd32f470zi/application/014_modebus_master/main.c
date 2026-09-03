#include <stdio.h>
#include <string.h>
#include <ohos_init.h>
#include "los_task.h"
#include "uart.h"
#include "queue_adapter.h"

#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6                   /* 值越大，优先级越高 */

#define MASTER_SLAVE_ADDR       0x01            /* 目标从机地址 */
#define MASTER_REQ_INTERVAL_MS  1000            /* 请求发送间隔 */
#define MASTER_RESP_TIMEOUT_MS  200             /* 响应等待超时 */
#define MASTER_RTU_MIN_SIZE     8
#define MODBUS_MASTER_TIMEOUT   0xFFFFFFFFU    /* 超时返回值 (非 LOS_OK) */

/* Modbus 请求帧: 01 03 00 00 00 0A C5 CD
   功能码 0x03 = Read Holding Registers, 起始地址 0x0000, 数量 0x000A */
static const uint8_t g_readReq[] = {
    0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD
};

/* CRC16-Modbus 查表 (多项式 0xA001, 初值 0xFFFF) */
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

static uint16_t modbus_crc16(const uint8_t *buf, uint32_t len)
{
    uint16_t crc = 0xFFFF;
    uint32_t i;
    for (i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc16_table[(crc ^ buf[i]) & 0xFF];
    }
    return crc;
}

/* 等待接收完整响应 (3.5 字符时间起算的简单轮询, 等待时间由调用方传入) */
static UINT32 wait_response(uint8_t *buf, UINT32 *size, UINT32 timeoutMs)
{
    UINT32 waited = 0;
    UINT32 step   = 10;  /* 10ms 轮询一次 */
    while (waited < timeoutMs) {
        *size = MAX_MSG_SIZE;
        if (rs485_2_mq_recv(buf, size) == LOS_OK && *size > 0) {
            return LOS_OK;
        }
        LOS_TaskDelay(step);
        waited += step;
    }
    return MODBUS_MASTER_TIMEOUT;  /* 超时 */
}

/* 处理 Modbus 响应: 校验、解析、打印 */
static void handle_response(const uint8_t *resp, UINT32 respLen)
{
    /* 长度校验 */
    if (respLen < MASTER_RTU_MIN_SIZE) {
        printf("[Master] response too short: %u\n", respLen);
        return;
    }

    /* 地址过滤: 只处理目标从机的响应 */
    if (resp[0] != MASTER_SLAVE_ADDR) {
        printf("[Master] foreign response addr=0x%02X, drop\n", resp[0]);
        return;
    }

    /* CRC 校验 */
    uint16_t crcCalc = modbus_crc16(resp, respLen - 2);
    uint16_t crcRecv = (uint16_t)resp[respLen - 2]
                     | ((uint16_t)resp[respLen - 1] << 8);
    if (crcCalc != crcRecv) {
        printf("[Master] CRC error: calc=0x%04X recv=0x%04X\n", crcCalc, crcRecv);
        return;
    }

    uint8_t funcCode = resp[1];

    /* 异常响应: 功能码最高位置 1 */
    if (funcCode & 0x80) {
        uint8_t exCode = (respLen >= 5) ? resp[2] : 0xFF;
        printf("[Master] exception: func=0x%02X code=0x%02X\n", funcCode & 0x7F, exCode);
        return;
    }

    /* 0x03 读保持寄存器响应: 地址(1) + 功能码(1) + 字节数(1) + 数据(N*2) + CRC(2) */
    if (funcCode == 0x03) {
        uint8_t  byteCount = resp[2];
        uint16_t regCount  = (uint16_t)(byteCount / 2);
        printf("[Master] read %u holding registers:\n", regCount);
        for (uint16_t i = 0; i < regCount; i++) {
            uint16_t val = ((uint16_t)resp[3 + i * 2] << 8) | resp[3 + i * 2 + 1];
            printf("  reg[%u] = 0x%04X (%u)\n", i, val, val);
        }
        return;
    }

    /* 其它功能码: 简单打印字节数 */
    printf("[Master] func=0x%02X, byteCount=%u, data:", funcCode,
           (respLen >= 5) ? resp[2] : 0);
    for (uint32_t i = 0; i < respLen; i++) {
        printf(" %02X", resp[i]);
    }
    printf("\n");
}

/* 直接使用常量表 g_readReq 作为发送帧 (含 CRC), 长度固定为 MASTER_REQ_LEN */
#define MASTER_REQ_LEN  sizeof(g_readReq)

static void *thread_modebus_master_task(unsigned int arg)
{
    (void)arg;
    UINT32 ret = 0;
    uint8_t  rxBuf[MAX_MSG_SIZE];
    UINT32   rxLen;

    while (1) {
        /* 1) 发送读保持寄存器请求 01 03 00 00 00 0A C5 CD */
        rs485_2_send((uint8_t *)g_readReq, MASTER_REQ_LEN);

        /* 2) 等待响应 (3.5 字符 ~4ms @ 9600, 这里留 200ms 充裕余量) */
        rxLen = 0;
        ret = wait_response(rxBuf, &rxLen, MASTER_RESP_TIMEOUT_MS);
        if (ret == MODBUS_MASTER_TIMEOUT) {
            printf("[Master] no response (timeout)\n");
        } else if (ret == LOS_OK) {
            /* 3) 解析并处理响应 */
            handle_response(rxBuf, rxLen);
        } else {
            printf("[Master] recv error: 0x%X\n", ret);
        }

        /* 4) 周期等待 */
        LOS_TaskDelay(MASTER_REQ_INTERVAL_MS);
    }

    return NULL;
}

static void modebus_master_task_init(void)
{
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {
        .pfnTaskEntry = thread_modebus_master_task,
        .uwStackSize = TASK_STACK_SIZE,
        .pcName = "modebusMasterTask",
        .usTaskPrio = TASK_PRIORITY,
    };
    if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate modebusMasterTask failed!\n");
        return;
    }
}

APP_FEATURE_INIT(modebus_master_task_init);
