/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
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

#include "rtc_adapter.h"
#include "rtc_time_hook.h"
#include <time.h>

#define RTC_CLOCK_SOURCE_LXTAL
#define BKP_VALUE    0x32F1

rtc_parameter_struct rtc_initpara;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;

void rtc_setup(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
void rtc_show_time(void);
uint8_t usart_input_threshold(uint32_t value);
void rtc_pre_config(void);

/*!
    \brief      init_rtc_hw function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int init_rtc_hw(void)
{
	/* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    rtc_pre_config();

    /* check if RTC has aready been configured */
    if (BKP_VALUE != RTC_BKP0){
        rtc_setup(2026, 8, 28, 8, 44, 59);
    }else{
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_BORRST)){
            printf("BOR reset flags\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)){
            printf("external reset flags\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_PORRST)){
            printf("power reset flags\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_SWRST)){
            printf("Software reset flags\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_FWDGTRST)){
            printf("FWDGT reset flags\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_WWDGTRST)){
            printf("WWDGT reset flags\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_LPRST)){
            printf("Low-power reset flags\n\r");
        }

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
void rtc_setup(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second)
{
    /* setup RTC time value */
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = DEC2BCD(year % 100);
    rtc_initpara.month = DEC2BCD(month);
    rtc_initpara.day_of_week = RTC_SUNDAY;
    rtc_initpara.date = DEC2BCD(date);
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;

    RTC_BKP1 = DEC2BCD(year / 100);
	rtc_initpara.hour = DEC2BCD(hour);
	rtc_initpara.minute = DEC2BCD(minute);
	rtc_initpara.second = DEC2BCD(second);

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
        printf("\n\r** RTC time configuration failed! **\n\r");
    }else{
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
    uint32_t time_subsecond = 0;
    uint8_t subsecond_ss = 0,subsecond_ts = 0,subsecond_hs = 0;

    rtc_current_time_get(&rtc_initpara);
    /* get the subsecond value of current time, and convert it into fractional format */
    time_subsecond = rtc_subsecond_get();
    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;

    printf("Current time: %0.2x-%0.2x-%0.2x %0.2x:%0.2x:%0.2x .%d%d%d \n\r", \
          rtc_initpara.year, rtc_initpara.month, rtc_initpara.date, rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second,\
          subsecond_ss, subsecond_ts, subsecond_hs);
	sync_rtc_time();
}
void sync_rtc_time(void)
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

UINT64 RtcGetTickHook(VOID)
{
    return 0;
}

INT32 RtcGetTimeHook(UINT64 *usec)
{
    return 0;
}

INT32 RtcSetTimeHook(UINT64 msec, UINT64 *usec)
{
    return 0;
}

INT32 RtcGetTimezoneHook(INT32 *tz)
{
    return 0;
}

INT32 RtcSetTimezoneHook(INT32 tz)
{
    return 0;
}

static struct RtcTimeHook rtchook = {
	.RtcGetTickHook = NULL,
	.RtcGetTimeHook = NULL,
	.RtcSetTimeHook = NULL,
	.RtcGetTimezoneHook = NULL,
	.RtcSetTimezoneHook = NULL,
};

void init_rtc(void)
{
	//初始化RTC硬件
	init_rtc_hw();
	//同步RTC时间
	sync_rtc_time();
	//注册RTC钩子函数
	LOS_RtcHookRegister(&rtchook);
}

/*!
    \brief      对外接口:设置 RTC 时间并同步系统时间
    \param[in]  year,month,date,hour,minute,second (十进制)
    \retval     0 成功, -1 失败
*/
int rtc_set_datetime(uint16_t year, uint8_t month, uint8_t date,
                     uint8_t hour, uint8_t minute, uint8_t second)
{
    /* 参数基本范围检查 */
    if (year < 2000 || year > 2099 || month < 1 || month > 12 ||
        date < 1 || date > 31 || hour > 23 || minute > 59 || second > 59) {
        return -1;
    }

    /* RTC 设置需要访问 Backup 域 */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();

    rtc_setup(year, month, date, hour, minute, second);

    /* 重新同步到系统时间 */
    sync_rtc_time();
    return 0;
}
