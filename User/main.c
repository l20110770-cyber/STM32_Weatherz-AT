#include "stm32f10x.h"                  // Device header
#include "ST7735S.h"
#include "Delay.h"
#include "BSP_ST7735S.h"
#include <stdio.h>
#include "ESP_AT.h"
#include "ST_Image.h"
#include "Task.h"
#include "Timer.h"
#include "RTC.h"
#include "MPU6050.h"


int main(void)
{
    /* 中断优先级分组：2bit抢占,2bit响应 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 系统时基定时器初始化（调度器心跳） */
    TIM2_Init();

    /* LCD初始化 */
    BSP_LCD_SPI_Init();
    ST7735S_Init();
    LCD_BLK_ON();
    ST7735S_FillColor(BLACK);

    /* 显示开机内容 */
    ST7735S_DrawString(0,0,"Initializing...",WHITE,BLACK);
    Delay_ms(500);

    /* RTC初始化 */
    RTC_Init();

    /* MPU6050初始化 */
    ST7735S_DrawString(0,16,"Init MPU6050...",WHITE,BLACK);
    MPU6050_Init();
    Delay_ms(500);

    /* ESP32 WiFi初始化 */
    ST7735S_DrawString(0,32,"Wait ESP_32...",WHITE,BLACK);
    Delay_ms(1500);

    WIFI_Init();

    ST7735S_DrawString(0,96,"Ready",GREEN,BLACK);
    Delay_ms(500);

    /* 清屏，后续由各任务独立绘制 */
    ST7735S_FillColor(BLACK);

    Task_InitialRun();
    while(1)
    {
        Task_RunSchedule();
    }
}
