#include <stdio.h>
#include <string.h>
#include <ohos_init.h>
#include "los_task.h"
#include "uart.h"
#include "queue_adapter.h"

#define TASK_STACK_SIZE     4096
#define TASK_PRIORITY       6                   /* 值越大，优先级越高 */

#define MODBUS_SLAVE_ADDR      0x01            /* 本机从机地址 */
#define MODBUS_REG_COUNT       10              /* 支持的保持/输入寄存器数量 */
#define MODBUS_COIL_COUNT      16              /* 支持的线圈/离散输入数量 */
#define MODBUS_RTU_MIN_SIZE    8               /* 最小合法 RTU 帧长度 (含 CRC) */

/* Modbus 异常码 */
#define MODBUS_EX_ILLEGAL_FUNCTION     0x01
#define MODBUS_EX_ILLEGAL_DATA_ADDR    0x02
#define MODBUS_EX_ILLEGAL_DATA_VALUE   0x03
#define MODBUS_EX_SLAVE_DEVICE_FAILURE 0x04

/* 响应 PDU 最大长度: 地址(1) + 功能码(1) + 字节数(1) + 数据(2*125) + CRC(2) */
#define MODBUS_RESP_MAX_SIZE           256

static uint8_t bufferAddr[MAX_MSG_SIZE];  /* 接收缓冲区 */
static UINT32 bufferSize = 0;            /* 接收缓冲区大小 */

/* Modbus 数据区 */
static uint16_t g_holdingRegs[MODBUS_REG_COUNT] = {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004,
    0x0005, 0x0006, 0x0007, 0x0008, 0x0009,
};
static uint16_t g_inputRegs[MODBUS_REG_COUNT] = {
    0x0100, 0x0101, 0x0102, 0x0103, 0x0104,
    0x0105, 0x0106, 0x0107, 0x0108, 0x0109,
};
static uint8_t  g_coils[MODBUS_COIL_COUNT]      = {0};   /* 线圈 (DO) */
static uint8_t  g_discreteInputs[MODBUS_COIL_COUNT] = {0,1,1,0,1,0,1,1,0,0,1,1,1,0,1,0}; /* 离散输入 (DI) */

/* CRC16-Modbus 查表 (多项式 0xA001, 初值 0xFFFF), 索引值 = (crc ^ byte) & 0xFF
   表项为按字节推进一字节后的新 CRC 值, 直接 XOR 当前 CRC 高字节即可得到下一步 CRC */
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

/* CRC16-Modbus 查表法计算 (多项式 0xA001, 初值 0xFFFF), 低字节在前 */
static uint16_t modbus_crc16(const uint8_t *buf, uint32_t len)
{
    uint16_t crc = 0xFFFF;
    uint32_t i;

    for (i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc16_table[(crc ^ buf[i]) & 0xFF];
    }

    return crc;
}

/* 追加 CRC 到 PDU 末尾, 返回追加后的总长度 (pdu_len + 2) */
static uint32_t modbus_append_crc(uint8_t *pdu, uint32_t pdu_len)
{
    uint16_t crc = modbus_crc16(pdu, pdu_len);
    pdu[pdu_len]     = (uint8_t)(crc & 0xFF);         /* CRC 低字节 */
    pdu[pdu_len + 1] = (uint8_t)((crc >> 8) & 0xFF); /* CRC 高字节 */
    return pdu_len + 2;
}

/* 构造异常响应 (地址 + 功能码|0x80 + 异常码 + CRC), 返回完整帧长度 */
static uint32_t modbus_build_exception(uint8_t *pdu, uint8_t addr, uint8_t func, uint8_t excode)
{
    pdu[0] = addr;
    pdu[1] = func | 0x80;
    pdu[2] = excode;
    return modbus_append_crc(pdu, 3);
}

/* 通用: 检查地址+数量范围 (start, count, total), 失败返回 0 */
static uint8_t modbus_check_range(uint16_t start, uint16_t count, uint16_t total)
{
    if (count == 0 || count > total) {
        return MODBUS_EX_ILLEGAL_DATA_VALUE;
    }
    if ((uint32_t)start + (uint32_t)count > total) {
        return MODBUS_EX_ILLEGAL_DATA_ADDR;
    }
    return 0;
}

/* ===================================================================
 *  各功能码处理函数:
 *  入参: req    = 请求帧 (含地址、功能码、数据、CRC, 即完整 RTU 帧)
 *        reqLen = 请求帧总长度 (含 CRC)
 *  出参: resp   = 响应 PDU 缓冲区 (不含 CRC), 也可用于异常响应
 *        respLen= 响应 PDU 长度 (不含 CRC)
 *  返回: 总响应长度 (含 CRC); 0 表示已就地生成异常, 调用方需改用异常路径
 * =================================================================== */

/* 0x01 Read Coils */
static uint32_t modbus_handle_01(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    (void)reqLen;
    uint16_t startAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t coilCount = ((uint16_t)req[4] << 8) | req[5];
    uint8_t  ex;

    ex = modbus_check_range(startAddr, coilCount, MODBUS_COIL_COUNT);
    if (ex != 0) {
        return modbus_build_exception(resp, req[0], req[1], ex);
    }

    uint8_t byteCount = (uint8_t)((coilCount + 7) / 8);
    resp[0] = req[0]; /* 设备地址 */
    resp[1] = req[1]; /* 功能码 */
    resp[2] = byteCount;
    memset(&resp[3], 0, byteCount);

    /* 线圈按位打包: 第 0 位是起始地址 */
    for (uint16_t i = 0; i < coilCount; i++) {
        if (g_coils[startAddr + i]) {
            resp[3 + (i / 8)] |= (uint8_t)(1u << (i % 8));
        }
    }
    *respLen = 3 + byteCount;
    return modbus_append_crc(resp, *respLen);
}

/* 0x02 Read Discrete Inputs */
static uint32_t modbus_handle_02(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    (void)reqLen;
    uint16_t startAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t inputCount = ((uint16_t)req[4] << 8) | req[5];
    uint8_t  ex;

    ex = modbus_check_range(startAddr, inputCount, MODBUS_COIL_COUNT);
    if (ex != 0) {
        return modbus_build_exception(resp, req[0], req[1], ex);
    }

    uint8_t byteCount = (uint8_t)((inputCount + 7) / 8);
    resp[0] = req[0];
    resp[1] = req[1];
    resp[2] = byteCount;
    memset(&resp[3], 0, byteCount);

    for (uint16_t i = 0; i < inputCount; i++) {
        if (g_discreteInputs[startAddr + i]) {
            resp[3 + (i / 8)] |= (uint8_t)(1u << (i % 8));
        }
    }
    *respLen = 3 + byteCount;
    return modbus_append_crc(resp, *respLen);
}

/* 0x03 Read Holding Registers */
static uint32_t modbus_handle_03(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    (void)reqLen;
    uint16_t startAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t regCount  = ((uint16_t)req[4] << 8) | req[5];
    uint8_t  ex;

    ex = modbus_check_range(startAddr, regCount, MODBUS_REG_COUNT);
    if (ex != 0) {
        return modbus_build_exception(resp, req[0], req[1], ex);
    }

    uint8_t byteCount = (uint8_t)(regCount * 2);
    resp[0] = req[0];
    resp[1] = req[1];
    resp[2] = byteCount;
    uint32_t idx = 3;
    for (uint16_t i = 0; i < regCount; i++) {
        uint16_t val = g_holdingRegs[startAddr + i];
        resp[idx++] = (uint8_t)(val >> 8);
        resp[idx++] = (uint8_t)(val & 0xFF);
    }
    *respLen = idx;
    return modbus_append_crc(resp, *respLen);
}

/* 0x04 Read Input Registers */
static uint32_t modbus_handle_04(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    (void)reqLen;
    uint16_t startAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t regCount  = ((uint16_t)req[4] << 8) | req[5];
    uint8_t  ex;

    ex = modbus_check_range(startAddr, regCount, MODBUS_REG_COUNT);
    if (ex != 0) {
        return modbus_build_exception(resp, req[0], req[1], ex);
    }

    uint8_t byteCount = (uint8_t)(regCount * 2);
    resp[0] = req[0];
    resp[1] = req[1];
    resp[2] = byteCount;
    uint32_t idx = 3;
    for (uint16_t i = 0; i < regCount; i++) {
        uint16_t val = g_inputRegs[startAddr + i];
        resp[idx++] = (uint8_t)(val >> 8);
        resp[idx++] = (uint8_t)(val & 0xFF);
    }
    *respLen = idx;
    return modbus_append_crc(resp, *respLen);
}

/* 0x05 Write Single Coil */
static uint32_t modbus_handle_05(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    (void)reqLen;
    uint16_t coilAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t coilVal  = ((uint16_t)req[4] << 8) | req[5];

    if (coilAddr >= MODBUS_COIL_COUNT) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_ADDR);
    }
    if (coilVal != 0x0000 && coilVal != 0xFF00) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_VALUE);
    }

    g_coils[coilAddr] = (coilVal == 0xFF00) ? 1 : 0;

    /* 正常响应: 原请求数据回显 */
    memcpy(resp, req, 6);
    *respLen = 6;
    return modbus_append_crc(resp, *respLen);
}

/* 0x06 Write Single Register */
static uint32_t modbus_handle_06(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    (void)reqLen;
    uint16_t regAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t regVal  = ((uint16_t)req[4] << 8) | req[5];

    if (regAddr >= MODBUS_REG_COUNT) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_ADDR);
    }

    g_holdingRegs[regAddr] = regVal;

    /* 正常响应: 原请求数据回显 */
    memcpy(resp, req, 6);
    *respLen = 6;
    return modbus_append_crc(resp, *respLen);
}

/* 0x0F Write Multiple Coils */
static uint32_t modbus_handle_0F(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    uint16_t startAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t coilCount = ((uint16_t)req[4] << 8) | req[5];
    uint8_t  byteCount = req[6];
    uint8_t  ex;

    ex = modbus_check_range(startAddr, coilCount, MODBUS_COIL_COUNT);
    if (ex != 0) {
        return modbus_build_exception(resp, req[0], req[1], ex);
    }
    if ((uint32_t)reqLen != (uint32_t)(7 + byteCount + 2) /* 含 CRC */) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_VALUE);
    }
    if (byteCount < (uint8_t)((coilCount + 7) / 8)) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_VALUE);
    }

    for (uint16_t i = 0; i < coilCount; i++) {
        uint8_t bit = (req[7 + (i / 8)] >> (i % 8)) & 0x01;
        g_coils[startAddr + i] = bit;
    }

    /* 正常响应: 地址(2) + 数量(2) */
    resp[0] = req[0];
    resp[1] = req[1];
    resp[2] = req[2];
    resp[3] = req[3];
    resp[4] = req[4];
    resp[5] = req[5];
    *respLen = 6;
    return modbus_append_crc(resp, *respLen);
}

/* 0x10 Write Multiple Registers */
static uint32_t modbus_handle_10(const uint8_t *req, uint32_t reqLen,
                                 uint8_t *resp, uint32_t *respLen)
{
    uint16_t startAddr = ((uint16_t)req[2] << 8) | req[3];
    uint16_t regCount  = ((uint16_t)req[4] << 8) | req[5];
    uint8_t  byteCount = req[6];
    uint8_t  ex;

    ex = modbus_check_range(startAddr, regCount, MODBUS_REG_COUNT);
    if (ex != 0) {
        return modbus_build_exception(resp, req[0], req[1], ex);
    }
    if (byteCount != (uint8_t)(regCount * 2)) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_VALUE);
    }
    if ((uint32_t)reqLen != (uint32_t)(7 + byteCount + 2)) {
        return modbus_build_exception(resp, req[0], req[1], MODBUS_EX_ILLEGAL_DATA_VALUE);
    }

    for (uint16_t i = 0; i < regCount; i++) {
        uint16_t val = ((uint16_t)req[7 + i * 2] << 8) | req[7 + i * 2 + 1];
        g_holdingRegs[startAddr + i] = val;
    }

    /* 正常响应: 地址(2) + 数量(2) */
    resp[0] = req[0];
    resp[1] = req[1];
    resp[2] = req[2];
    resp[3] = req[3];
    resp[4] = req[4];
    resp[5] = req[5];
    *respLen = 6;
    return modbus_append_crc(resp, *respLen);
}

typedef uint32_t (*modbus_handler_t)(const uint8_t *req, uint32_t reqLen,
                                     uint8_t *resp, uint32_t *respLen);

static const struct {
    uint8_t          funcCode;
    modbus_handler_t handler;
} g_modbus_handlers[] = {
    { 0x01, modbus_handle_01 },
    { 0x02, modbus_handle_02 },
    { 0x03, modbus_handle_03 },
    { 0x04, modbus_handle_04 },
    { 0x05, modbus_handle_05 },
    { 0x06, modbus_handle_06 },
    { 0x0F, modbus_handle_0F },
    { 0x10, modbus_handle_10 },
};
#define MODBUS_HANDLER_COUNT  (sizeof(g_modbus_handlers) / sizeof(g_modbus_handlers[0]))

static void *thread_modebus_slave_task(unsigned int arg)
{
    UINT32 ret = 0;
    uint8_t respBuf[MODBUS_RESP_MAX_SIZE] = {0};

    while (1) {
        bufferSize = MAX_MSG_SIZE;
        ret = rs485_1_mq_recv(bufferAddr, &bufferSize);
        if (ret != LOS_OK) {
            LOS_TaskDelay(100);
            continue;
        }

        /* 长度校验 */
        if (bufferSize < MODBUS_RTU_MIN_SIZE || bufferSize > 256) {
            continue;
        }

        uint8_t slaveAddr = bufferAddr[0];
        uint8_t funcCode  = bufferAddr[1];

        /* 地址过滤 */
        if (slaveAddr != MODBUS_SLAVE_ADDR) {
            continue;
        }

        /* CRC 校验 */
        uint16_t crcCalc = modbus_crc16(bufferAddr, bufferSize - 2);
        uint16_t crcRecv = (uint16_t)bufferAddr[bufferSize - 2]
                         | ((uint16_t)bufferAddr[bufferSize - 1] << 8);
        if (crcCalc != crcRecv) {
            continue;
        }

        /* 按功能码分发 */
        modbus_handler_t handler = NULL;
        for (uint32_t i = 0; i < MODBUS_HANDLER_COUNT; i++) {
            if (g_modbus_handlers[i].funcCode == funcCode) {
                handler = g_modbus_handlers[i].handler;
                break;
            }
        }

        uint32_t sendLen;
        if (handler != NULL) {
            uint32_t respLen = 0;
            sendLen = handler(bufferAddr, bufferSize, respBuf, &respLen);
        } else {
            /* 不支持的功能码 -> Illegal Function (0x01) */
            sendLen = modbus_build_exception(respBuf, slaveAddr, funcCode, MODBUS_EX_ILLEGAL_FUNCTION);
        }

        rs485_1_send(respBuf, sendLen);
    }

    return NULL;
}

static void modebus_slave_task_init(void)
{
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {
        .pfnTaskEntry = thread_modebus_slave_task,
        .uwStackSize = TASK_STACK_SIZE,
        .pcName = "modebusSlaveTask",
        .usTaskPrio = TASK_PRIORITY,
    };
    if (LOS_TaskCreate(&taskID, &stTask) != LOS_OK) {
        printf("** LOS_TaskCreate modebusSlaveTask failed!\n");
        return;
    }
}

APP_FEATURE_INIT(modebus_slave_task_init);
