#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ohos_init.h>
#include "los_task.h"
#if (LOSCFG_USE_SHELL == 1)
#include "shell.h"
#include "shcmd.h"
#endif
#include "rtc_adapter.h"

/*启用shell命令 */
#if (LOSCFG_USE_SHELL == 1)

INT32 cmd_date_set(INT32 argc, const CHAR **argv)
{
    if (argc != 1) {
        printf("Usage: date_set \"YYYY-MM-DD HH:MM:SS\"\n");
        return LOS_NOK;
    }

    const char *p = argv[0];
    char *end;
    long val;

    /* 解析年份 YYYY- */
    val = strtol(p, &end, 10);
    if (end == p || *end != '-' || val < 1970 || val > 2100)
        goto err;
    int year = (int)val;
    p = end + 1;

    /* 解析月份 MM- */
    val = strtol(p, &end, 10);
    if (end == p || *end != '-' || val < 1 || val > 12)
        goto err;
    int month = (int)val;
    p = end + 1;

    /* 解析日期 DD<空格> */
    val = strtol(p, &end, 10);
    if (end == p || *end != ' ' || val < 1 || val > 31)
        goto err;
    int date = (int)val;
    p = end + 1;

    /* 解析小时 HH: */
    val = strtol(p, &end, 10);
    if (end == p || *end != ':' || val < 0 || val > 23)
        goto err;
    int hour = (int)val;
    p = end + 1;

    /* 解析分钟 MM: */
    val = strtol(p, &end, 10);
    if (end == p || *end != ':' || val < 0 || val > 59)
        goto err;
    int minute = (int)val;
    p = end + 1;

    /* 解析秒数 SS */
    val = strtol(p, &end, 10);
    if (end == p || *end != '\0' || val < 0 || val > 60)
        goto err;
    int second = (int)val;

    /* 写入 RTC */
    if (rtc_set_datetime((uint16_t)year, (uint8_t)month, (uint8_t)date,
                         (uint8_t)hour, (uint8_t)minute, (uint8_t)second) != 0) {
        printf("date_set failed: out of range or RTC write error\n");
        return LOS_NOK;
    }
    return LOS_OK;

err:
    printf("Invalid format. Usage: date_set \"YYYY-MM-DD HH:MM:SS\"\n");
    return LOS_NOK;
}

INT32 cmd_reboot(INT32 argc, const CHAR **argv)
{
    if (argc != 0) {
        printf("Usage: reboot\n");
        return LOS_NOK;
    }
	LOS_TaskLock();
	LOS_IntLock();
	NVIC_SystemReset();
    return LOS_OK;
}

static void shell_cmd_init(void) 
{
    // 初始化shell命令
    OsShellInit();
    // 注册日期设置命令
    osCmdReg(CMD_TYPE_EX, "date_set", 6, (CMD_CBK_FUNC)cmd_date_set);
    // 注册重启命令
    osCmdReg(CMD_TYPE_EX, "reboot", 0, (CMD_CBK_FUNC)cmd_reboot);
}

SYS_RUN(shell_cmd_init);
#endif
