#include "stm32f10x.h"                  // Device header
#include "Timer.h"
#include "ESP_AT.h"
#include "RTC.h"
#include "ST7735S.h"
#include "Weather.h"
#include "MPU6050.h"


typedef struct
{
    uint32_t cycle_ms;
    uint32_t Last_Time;
    void (*Task_func)(void);
}TaskTypeDef;

/*100ms刷新时间画面*/
void Task_TimeRefresh(void)
{
    static uint8_t blink_counter = 0;
    static bool colon_show = true;
    static uint16_t last_day = 0;
    char str_buffer[32];

    /*从STM32底层硬件读取最新时间到RTC_Time数组中*/
    RTC_GetTime();

    /* ---- 左上角：日期 (8x16字体) ---- */
    if(RTC_Time[2] != last_day)
    {
        last_day = RTC_Time[2];
        snprintf(str_buffer,sizeof(str_buffer),"%04d/%02d/%02d",
                 RTC_Time[0],RTC_Time[1],RTC_Time[2]);
        ST7735S_DrawString(0,0,str_buffer,WHITE,BLACK);
    }

    /* ---- 底部居中：时间 (16x32大字体, "HH:MM" = 5char*16px = 80px, 居中x=(128-80)/2=24) ---- */
    blink_counter++;
    if(blink_counter >= 5)
    {
        blink_counter = 0;
        colon_show = !colon_show;
    }

    if(colon_show)
    {
        snprintf(str_buffer,sizeof(str_buffer),"%02d:%02d",RTC_Time[3],RTC_Time[4]);
    }
    else
    {
        snprintf(str_buffer,sizeof(str_buffer),"%02d %02d",RTC_Time[3],RTC_Time[4]);
    }

    ST7735S_DrawString_16X32(24,96,str_buffer,BLUE,BLACK);
}

/*10min更新天气*/
void Task_WeatherUpdata(void)
{
    weather_t current_weather;
    if(Weather_Fetch(&current_weather))
    {
        Weather_Display(&current_weather);
    }
}

/*10s更新环境温度 (右上区域, 温度下方) */
void Task_EnvTemp(void)
{
    char temp_buf[20]={0};
    MPU6050_DataTypeDef MPU_Data;
    MPU6050_GetData(&MPU_Data);
    float temper = MPU_Data.Temp;
    snprintf(temp_buf,sizeof(temp_buf),"%.2fC",temper);
    ST7735S_DrawString(80,52,temp_buf,CYAN,BLACK);
}

/* 北京时间 = UTC+8 */
#define TIMEZONE_OFFSET_SEC   (8*3600)

/*1hNTP校时：联网同步时间*/
void Task_NTPSync(void)
{
    uint32_t net_timestamp = 0;
    
    if(ESP_AT_TimeGet(&net_timestamp))
    {
        /* AT+SYSTIMESTAMP? 返回的是标准UTC时间戳, 不受AT+CIPSNTPCFG里配置的
           时区影响, 这里手动加上东八区偏移换算成北京本地时间再写入RTC */
        RTC_Set_timestamp(net_timestamp + TIMEZONE_OFFSET_SEC);
    }
}

static TaskTypeDef task_list[]=
{
    {100,       0,Task_TimeRefresh},
    {10000,     0,Task_EnvTemp},
    {10*60*1000,0,Task_WeatherUpdata},
    {60*60*1000,0,Task_NTPSync}
};

#define TASK_NUM (sizeof(task_list)/sizeof(TaskTypeDef))

/* 开机后立即执行一次所有任务，让屏幕一开始就显示完整信息，
   而不用等待各任务自己的周期(尤其NTP是1小时、天气是10分钟)。
   在main()里 WIFI_Init() 成功之后、进入 while(1) 主循环之前调用一次即可。 */
void Task_InitialRun(void)
{
    uint32_t now;
    uint8_t i;

    /* 先做一次NTP校时，这样紧接着刷新的时间才是同步后的准确时间，
       而不是RTC默认的 2024-01-01 00:00:00 */
    Task_NTPSync();

    /* 把所有任务的计时基准同步到当前时刻，避免手动执行完之后，
       调度器在下一次 Task_RunSchedule() 里因为 Last_Time 还是0 而立刻重复执行一次 */
    now = GetSysTick();
    for(i = 0;i < TASK_NUM;i++)
    {
        task_list[i].Last_Time = now;
    }

    Task_TimeRefresh();
    Task_EnvTemp();
    Task_WeatherUpdata();
    /* Task_NTPSync 已经在前面手动执行过了，这里不用再调用 */
}

void Task_RunSchedule(void)
{
    uint32_t now = GetSysTick();
    for(uint8_t i = 0;i < TASK_NUM;i++)
    {
        if(now - task_list[i].Last_Time >= task_list[i].cycle_ms)
        {
            task_list[i].Last_Time = now;
            task_list[i].Task_func();
        }
    }
}
