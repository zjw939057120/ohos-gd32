#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ohos_init.h>
#include "los_task.h"
#include "shell.h"
#include "shcmd.h"
#include "rtc_adapter.h"


/* 去掉字符串首尾的双引号(若有) */
static void strip_quote(char *s)
{
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

INT32 cmd_date_set(INT32 argc, const CHAR **argv)
{
    if (argc != 1) {
        printf("Usage: date_set \"YYYY-MM-DD HH:MM:SS\"\n");
        return LOS_NOK;
    }

    char buf[32] = {0};
    strncpy(buf, argv[0], sizeof(buf) - 1);
    strip_quote(buf);

    int year, month, date, hour, minute, second;
    int n = sscanf(buf, "%d-%d-%d %d:%d:%d",
                   &year, &month, &date, &hour, &minute, &second);
    if (n != 6) {
        printf("Invalid format. Usage: date_set \"YYYY-MM-DD HH:MM:SS\"\n");
        return LOS_NOK;
    }

    if (rtc_set_datetime((uint16_t)year, (uint8_t)month, (uint8_t)date,
                         (uint8_t)hour, (uint8_t)minute, (uint8_t)second) != 0) {
        printf("date_set failed: out of range or RTC write error\n");
        return LOS_NOK;
    }
    return LOS_OK;
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

APP_FEATURE_INIT(shell_cmd_init);
