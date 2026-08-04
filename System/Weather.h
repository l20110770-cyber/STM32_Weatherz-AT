#ifndef   __WEATHER_H
#define   __WEATHER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    char city[16];                  //城市名（如”Hefei”）
    char weather_desc[16];          //天气英文描述（如”Cloudy”）
    char weather_temperature[8];    //温度字符串（如”27”）
    int weather_code;               //原始天气代码（如 4）
}weather_t;

/*发起网络请求并解析天气数据*/
bool Weather_Fetch(weather_t *weather);

/*在屏幕上绘制天气图标和数据*/
void Weather_Display(weather_t *weather);
#endif
