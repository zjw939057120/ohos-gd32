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
 * LED GPIO driver for GD32F470ZI.
 * LED1 on PA8, LED2 on PE3, both active high (high level = LED on).
 * Push-pull output, no pull-up/down, 50MHz drive. LEDs start off.
 */

#include "rtc.h"
#include <time.h>

#define RTC_CLOCK_SOURCE_LXTAL 
#define BKP_VALUE    0x32F1

rtc_parameter_struct   rtc_initpara;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;
uint32_t RTCSRC_FLAG = 0;

void rtc_show_time(void);
void rtc_pre_config(void);

int RtcInit(void)
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();

    rtc_pre_config();
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
    if ((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)){
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program 
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
        
        uint16_t year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        parse_compile_time(&year, &month, &day, &hour, &minute, &second);
        rtc_setup(year, month, day, hour, minute, second);
    }else{
        /* detect the reset source */
        if(RESET != rcu_flag_get(RCU_FLAG_PORRST)){
            printf("power on reset occurred....\n\r");
        }else if(RESET != rcu_flag_get(RCU_FLAG_EPRST)){
            printf("external reset occurred....\n\r");
        }
        printf("no need to configure RTC....\n\r");
        rtc_show_time();
    }

    rcu_all_reset_flag_clear();
}

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC32K) 
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
  
          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    
          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(uint16_t year, uint8_t date, uint8_t month, uint8_t hour, uint8_t minute, uint8_t second)
{
    /* setup RTC time value */
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = DEC2BCD(year % 100);
    rtc_initpara.month = DEC2BCD(month);
    rtc_initpara.date = DEC2BCD(date);
    rtc_initpara.display_format = RTC_24HOUR;

    RTC_BKP1 = DEC2BCD(year / 100);
	rtc_initpara.hour = DEC2BCD(hour);
	rtc_initpara.minute = DEC2BCD(minute);
	rtc_initpara.second = DEC2BCD(second);

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
        printf("\n\r** RTC time configuration failed! **\n\r");
    }else{
        printf("\n\r** RTC time configuration success! **\n\r");
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }
}

/*!
    \brief      display the current time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_time(void)
{
    rtc_current_time_get(&rtc_initpara);  
    printf("Current time: %0.2x%0.2x-%0.2x-%0.2x %0.2x:%0.2x:%0.2x\n\r", \
          RTC_BKP1, rtc_initpara.year, rtc_initpara.month, rtc_initpara.date, rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
    SetSysTime();
}

/*!
    \brief      convert RTC time to system time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SetSysTime(void)
{
	struct tm tm_time = {0};
	struct timespec ts = {0};

	/* RTC year 寄存器存放两位数年份,以 2000 为基准补齐到完整年份 */
	tm_time.tm_year = BCD2DEC(RTC_BKP1) * 100 + BCD2DEC(rtc_initpara.year) - 1900;
	tm_time.tm_mon  = BCD2DEC(rtc_initpara.month) - 1;
	tm_time.tm_mday = BCD2DEC(rtc_initpara.date);
	tm_time.tm_hour = BCD2DEC(rtc_initpara.hour);
	tm_time.tm_min  = BCD2DEC(rtc_initpara.minute);
	tm_time.tm_sec  = BCD2DEC(rtc_initpara.second);
	tm_time.tm_isdst = -1;

	/* 将 RTC 时间转换为自 1970-01-01 起的秒数 */
	ts.tv_sec = mktime(&tm_time);
	ts.tv_nsec = 0;

	if (clock_settime(CLOCK_REALTIME, &ts) != 0) {
		printf("sync time failed\n");
	}
}

/**
 * @brief 解析编译时间
 * 
 * @param year 年指针
 * @param month 月指针
 * @param date 日指针
 * @param hour 小时指针
 * @param minute 分钟指针
 * @param second 秒指针
 */
void parse_compile_time(uint16_t *year, uint8_t *month, uint8_t *day, 
                        uint8_t *hour, uint8_t *minute, uint8_t *second) {
    const char *date_str = __DATE__; // 格式: "Mmm dd yyyy"
    const char *time_str = __TIME__; // 格式: "hh:mm:ss"

    // 1. 解析年份 (取字符串第 7~10 位)
    *year = (date_str[7] - '0') * 1000 + (date_str[8] - '0') * 100 + 
            (date_str[9] - '0') * 10 + (date_str[10] - '0');

    // 2. 解析月份 (通过比对前 3 个字符的缩写)
    const char months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    *month = 1; // 默认值
    for (int i = 0; i < 12; i++) {
        if (strncmp(date_str, months[i], 3) == 0) {
            *month = i + 1;
            break;
        }
    }

    // 3. 解析日期 (取字符串第 4~5 位，注意处理前导空格)
    // 如果第 4 位是空格，说明是个位数日期；否则按两位数计算
    if (date_str[4] == ' ') {
        *day = date_str[5] - '0';
    } else {
        *day = (date_str[4] - '0') * 10 + (date_str[5] - '0');
    }

    // 4. 解析时、分、秒 (利用固定格式 "hh:mm:ss" 直接通过下标计算)
    *hour   = (time_str[0] - '0') * 10 + (time_str[1] - '0');
    *minute = (time_str[3] - '0') * 10 + (time_str[4] - '0');
    *second = (time_str[6] - '0') * 10 + (time_str[7] - '0');
}

int SetRtcTime(uint16_t year, uint8_t month, uint8_t date,
                     uint8_t hour, uint8_t minute, uint8_t second)
{
    if (year < 1970 || year > 2099 || month < 1 || month > 12 ||
        date < 1 || date > 31 || hour > 23 || minute > 59 || second > 59) {
        return -1;
    }

    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();

    rtc_pre_config();

    rtc_setup(year, month, date, hour, minute, second);

    rcu_all_reset_flag_clear();

    SetSysTime();
    return 0;
}

