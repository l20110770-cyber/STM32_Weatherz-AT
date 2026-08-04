#include "stm32f10x.h"                  // Device header
#include <stdlib.h>
#include <string.h>
#include "weather.h"
#include "ESP_AT.h"
#include "ST7735S.h"
#include "ST_Image.h"

extern const char *weather_uri;

/* ========== UI 布局坐标 (128x128 TFT) ========== */

/* --- 左上区域：图标 + 描述 --- */
#define W_ICON_X            2
#define W_ICON_Y            18

/* --- 右上区域：城市 / 温度 --- */
#define W_CITY_X            88
#define W_CITY_Y            0

#define W_TEMP_X            72
#define W_TEMP_Y            18

/* --- 天气描述 (图标下方) --- */
#define W_DESC_X            2
#define W_DESC_Y            68

/* --- 环境温度 (右上, 温度下方) --- */
#define W_ENV_X             72
#define W_ENV_Y             52

/*JSON 字符串提取器*/
static bool Extract_JSON_Value(const char *json_str,const char *key,char *out_buffer,uint16_t max_len)
{
    char search_key[32];
    snprintf(search_key,sizeof(search_key),"\"%s\":\"",key);

    char *start_ptr = strstr(json_str,search_key);
    if(start_ptr == NULL) return false;

    start_ptr += strlen(search_key);
    char *end_ptr = strchr(start_ptr,'\"');
    if(end_ptr == NULL) return false;

    uint16_t copy_len = end_ptr - start_ptr;
    if(copy_len >= max_len) copy_len = max_len -1;

    strncpy(out_buffer,start_ptr,copy_len);
    out_buffer[copy_len] = '\0';
    return true;
}

/*解析JSON存入结构体*/
static bool weather_parse(const char *json_str,weather_t *weather_data)
{
    char code_str[10]={0};
    if(json_str == NULL || weather_data == NULL) return false;

    /* 心知天气 API v3 返回 "results" 数组, 数据在 "now" 对象内 */
    if(strstr(json_str,"\"now\":") == NULL) return false;

    /* 提取城市名 */
    if(!Extract_JSON_Value(json_str,"name",weather_data->city,sizeof(weather_data->city)))
    {
        strcpy(weather_data->city,"--");
    }

    /* 提取天气描述 */
    if(!Extract_JSON_Value(json_str,"text",weather_data->weather_desc,sizeof(weather_data->weather_desc)))
    {
        strcpy(weather_data->weather_desc,"Unknown");
    }

    /* 提取温度 */
    if(!Extract_JSON_Value(json_str,"temperature",weather_data->weather_temperature,sizeof(weather_data->weather_temperature)))
    {
        strcpy(weather_data->weather_temperature,"--");
    }

    /* 提取天气代码 */
    if(Extract_JSON_Value(json_str,"code",code_str,sizeof(code_str)))
    {
        weather_data->weather_code = atoi(code_str);
    }
    else
    {
        weather_data->weather_code = -1;
    }
    return true;
}

/*根据天气代码匹配图标*/
static const ST_Image_t *Weather_GetIcon(int code)
{
    if(code >= 0 && code <= 3)      return &img_Sunny;
    if(code >= 4 && code <= 8)      return &img_Cloudy;
    if(code == 9)                   return &img_yin;
    if(code >= 10 && code <= 20)    return &img_Rainy;
    if(code >= 21 && code <= 25)    return &img_Snow;
    if(code >= 32 && code <= 36)    return &img_Wind;

    return &img_Cloudy;
}

/*拉取并解析天气*/
bool Weather_Fetch(weather_t *weather)
{
    const char *rsp = NULL;
    uint32_t length = 0;

    /*发起HTTP GET请求*/
    if(!ESP_AT_httpGet (weather_uri,&rsp,&length,10000))
    {
        return false;
    }

    /*解析 JSON*/
    if(!weather_parse(rsp,weather))
    {
        return false;
    }
    return true;
}

/*绘制天气UI*/
void Weather_Display(weather_t *weather)
{
    const ST_Image_t *img;
    char disp_buf[20];

    if(weather == NULL) return;

    img = Weather_GetIcon(weather->weather_code);

    /* ---- 左上：天气图标 (48x48) ---- */
    if(img != NULL)
    {
        /* 清除图标区域 */
        ST7735S_FillRectangel(W_ICON_X, W_ICON_Y, 48, 48, BLACK);
        ST7735S_ShowImage(W_ICON_X, W_ICON_Y, img->Width, img->Height,
                          (const uint16_t *)img->data);
    }

    /* ---- 左上：天气描述 (图标下方) ---- */
    ST7735S_DrawString(W_DESC_X, W_DESC_Y, weather->weather_desc, YELLOW, BLACK);

    /* ---- 右上：城市名 ---- */
    //ST7735S_DrawString(W_CITY_X, W_CITY_Y, weather->city, CYAN, BLACK);
    ST7735S_DrawString_Mix(W_CITY_X,W_CITY_Y,"合肥",CYAN,BLACK);

    /* ---- 右上：温度 (大号16x32字体) ---- */
    snprintf(disp_buf, sizeof(disp_buf), "%sC", weather->weather_temperature);
    ST7735S_DrawString_16X32(W_TEMP_X, W_TEMP_Y, disp_buf, YELLOW, BLACK);
}




