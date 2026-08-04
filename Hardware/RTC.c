#include "stm32f10x.h"  // Device header
#include "RTC.h"

uint16_t RTC_Time[6]={0};

void RTC_SetTime(void);

/* ---------- 纯手写的日历<->Unix时间戳换算，替代 <time.h> ---------- */

static const uint8_t days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

static bool is_leap_year(uint16_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* 日历时间 -> Unix时间戳(自1970-01-01 00:00:00起的秒数) */
static uint32_t Calendar_To_Timestamp(uint16_t year,uint8_t month,uint8_t day,
                                       uint8_t hour,uint8_t min,uint8_t sec)
{
    uint32_t days = 0;
    uint16_t y;
    uint8_t m;

    for(y = 1970; y < year; y++)
    {
        days += is_leap_year(y) ? 366 : 365;
    }

    for(m = 1; m < month; m++)
    {
        days += days_in_month[m-1];
        if(m == 2 && is_leap_year(year)) days += 1;
    }

    days += (day - 1);

    return days*86400UL + (uint32_t)hour*3600UL + (uint32_t)min*60UL + sec;
}

/* Unix时间戳 -> 日历时间 */
static void Timestamp_To_Calendar(uint32_t timestamp,uint16_t *year,uint8_t *month,uint8_t *day,
                                   uint8_t *hour,uint8_t *min,uint8_t *sec)
{
    uint32_t days = timestamp / 86400UL;
    uint32_t rem  = timestamp % 86400UL;
    uint16_t y = 1970;
    uint8_t m;

    *hour = (uint8_t)(rem / 3600);
    rem %= 3600;
    *min  = (uint8_t)(rem / 60);
    *sec  = (uint8_t)(rem % 60);

    while(1)
    {
        uint16_t year_days = is_leap_year(y) ? 366 : 365;
        if(days < year_days) break;
        days -= year_days;
        y++;
    }
    *year = y;

    for(m = 1; m <= 12; m++)
    {
        uint8_t md = days_in_month[m-1];
        if(m == 2 && is_leap_year(y)) md += 1;
        if(days < md) break;
        days -= md;
    }
    *month = m;
    *day = (uint8_t)(days + 1);
}

/* ---------- RTC 接口 ---------- */

void RTC_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);

    PWR_BackupAccessCmd(ENABLE);

    if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        RCC_LSEConfig(RCC_LSE_ON);
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET );

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();
        RTC_WaitForLastTask();

        RTC_SetPrescaler (32768-1);
        RTC_WaitForLastTask();

        /* 首次上电时设置默认时间 2024-01-01 00:00:00 */
        RTC_Time[0] = 2024;
        RTC_Time[1] = 1;
        RTC_Time[2] = 1;
        RTC_Time[3] = 0;
        RTC_Time[4] = 0;
        RTC_Time[5] = 0;

        RTC_SetTime();
        BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
    }
    else
    {
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
    }
}

void RTC_SetTime(void)
{
    uint32_t timestamp = Calendar_To_Timestamp(RTC_Time[0],(uint8_t)RTC_Time[1],(uint8_t)RTC_Time[2],
                                                (uint8_t)RTC_Time[3],(uint8_t)RTC_Time[4],(uint8_t)RTC_Time[5]);
    RTC_WaitForLastTask();
    RTC_SetCounter(timestamp);
    RTC_WaitForLastTask();
}

void RTC_GetTime(void)
{
    uint32_t timestamp = RTC_GetCounter();
    uint16_t year;
    uint8_t month,day,hour,min,sec;

    Timestamp_To_Calendar(timestamp,&year,&month,&day,&hour,&min,&sec);

    RTC_Time[0] = year;
    RTC_Time[1] = month;
    RTC_Time[2] = day;
    RTC_Time[3] = hour;
    RTC_Time[4] = min;
    RTC_Time[5] = sec;
}

void RTC_Set_timestamp(uint32_t timestamp)
{
    RTC_WaitForLastTask();
    RTC_SetCounter(timestamp);
    RTC_WaitForLastTask();
}

void RTC_Get_timestamp(uint32_t *timestamp)
{
    if(timestamp)
    {
        *timestamp = RTC_GetCounter();
    }
}
