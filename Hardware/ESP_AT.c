#include <stdbool.h>
#include <stdint.h>
#include <ESP_USART.h>
#include <Delay.h>
#include <ESP_AT.h>
#include "ST7735S.h"
#include "BSP_ST7735S.h"

#define RX_Buffer_Size          4096

#define Rx_Result_OK            0
#define Rx_Result_Error         1
#define Rx_Result_Fail          2

static uint8_t RxData[RX_Buffer_Size];
static uint32_t RxLength;
static bool RxReady;
static uint8_t RxResult;
const char *wifi_ssid = "ChinaNet-hX6DPE";
const char *wifi_password = "kuby2427";
const char *weather_uri = "https://api.seniverse.com/v3/weather/now.json?key=SXsJA6AyhkxXYVoY2&location=hefei&language=en&unit=c";




static void ON_USART_Receive(uint8_t Data)
{
    //没有数据请求，不接受数据
    if(!RxReady)
    {
        return;
    }
    
    //接收数据，防止缓冲区溢出
    if(RxLength < RX_Buffer_Size )
    {
        RxData [RxLength ++] = Data;
    }
    else
    {
        RxResult = Rx_Result_Fail;
        RxReady = false;
        return;
    }
    
    //数据接收完毕判断
    if(Data == '\n')
    {
        //接收换行符是否为\r\n
        if(RxLength >= 2 && RxData[RxLength - 2] == '\r')
        {
            if(RxLength >= 4 &&
                RxData[RxLength - 4] == 'O' && RxData[RxLength - 3] == 'K')
            {
                RxResult = Rx_Result_OK;
                RxReady = false;
            }
            else if(RxLength >=7 && 
                RxData[RxLength-5] == 'E' && RxData[RxLength-4] == 'R' && 
                RxData[RxLength-3] == 'R' && RxData[RxLength-2] == 'O' &&
                RxData[RxLength-1] == 'R')
            {
                RxResult = Rx_Result_Error;
                RxReady = false;
            }
        }
        
    }
}

bool ESP_AT_Init(void)
{
    RxReady = false ;

    ESP_USART_Init ();
    ESP_USART_Receive_Register (ON_USART_Receive);
    return true;
}


bool ESP_AT_SendCmd(const char *Cmd,const char **rsp,uint32_t *Length,uint32_t Timeout)
{
    RxLength = 0;
    RxReady = true;
    RxResult = Rx_Result_Fail;
    
    ESP_USART_W_String(Cmd);
    ESP_USART_W_String("\r\n");
    
    while(RxReady && Timeout--)
    {
        Delay_ms(1);
    }
    RxReady = false;
    
    if(RxLength < RX_Buffer_Size) RxData[RxLength] = '\0';
    else RxData[RX_Buffer_Size - 1] = '\0';
    
    if(rsp)
    {
        *rsp = (const char *)RxData;
    }
    if(Length)
    {
        *Length = RxLength;
    }

    
    return RxResult == Rx_Result_OK;
}
bool ESP_AT_SendData(const uint8_t *Data,uint32_t Length)
{
    ESP_USART_W_Data((uint8_t *)Data,Length);
    
    return true;
}

bool ESP_AT_Reset(void)
{
    /* 先用简单AT测试ESP是否在线 */
    if(!ESP_AT_SendCmd("AT",NULL,NULL,1000))
    {
        /* 通信失败，尝试恢复出厂并等待重启 */
        ESP_AT_SendCmd("AT+RESTORE",NULL,NULL,3000);
        Delay_ms(5000);   /* ESP32-C3 重启+NVS重建需要较长时间 */

        /* 重启后重新测试 */
        if(!ESP_AT_SendCmd("AT",NULL,NULL,2000))
        {
            return false;
        }
    }

    /* 关闭回显 */
    if(!ESP_AT_SendCmd("ATE0",NULL,NULL,1000))
    {
        return false;
    }

    /* 关闭存储(配置不写入Flash) */
    if(!ESP_AT_SendCmd("AT+SYSSTORE=0",NULL,NULL,1000))
    {
        return false;
    }

    return true;
}

bool ESP_AT_WIFIInit(void)
{
    if(!ESP_AT_SendCmd("AT+CWMODE=1",NULL,NULL,1000))
    {
        return false;
    }
    
    return true;
}    

bool ESP_AT_WIFIConect(const char *ssid,const char *pwd)
{
    char Cmd[64];
    
    //连接WiFi
    snprintf(Cmd,sizeof(Cmd),"AT+CWJAP=\"%s\",\"%s\"",ssid,pwd);
    if(!ESP_AT_SendCmd(Cmd,NULL,NULL,15000))
    {
        return false;
    }
    
    return true;
    
}

bool ESP_AT_httpGet(const char *url,const char **rsp,uint32_t *Length,uint32_t Timeout)
{
    char Cmd[128];

    snprintf(Cmd,sizeof(Cmd),"AT+HTTPCGET=\"%s\"",url);
    if(!ESP_AT_SendCmd (Cmd,rsp,Length,Timeout))
    {
        return false;
    }

    return true;
}    

bool ESP_AT_sntpInit(void)
{
    /* 配置SNTP: 使能, 时区UTC+8, NTP服务器 */
    if(!ESP_AT_SendCmd("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"",NULL,NULL,2000))
    {
        return false;
    }

    /* 设置同步间隔为1小时 */
    if(!ESP_AT_SendCmd("AT+CIPSNTPINTV=3600",NULL,NULL,1000))
    {
        return false;
    }

    /* 等待时间同步完成 (+TIME_UPDATED) */
    Delay_ms(3000);

    /* 查询SNTP时间，验证同步成功 */
    if(!ESP_AT_SendCmd("AT+CIPSNTPTIME?",NULL,NULL,2000))
    {
        return false;
    }

    return true;
}


bool ESP_AT_TimeGet(uint32_t *timestamp)
{
    const char *rsp;
    uint32_t Length;
    
    if(!ESP_AT_SendCmd("AT+SYSTIMESTAMP?",&rsp,&Length,1000))
    {
        return false;
    }
    
    char *sts = strstr(rsp,"+SYSTIMESTAMP:");
    if(sts == NULL)
    {
        return false;
    }
    sts +=strlen("+SYSTIMESTAMP:");
    *timestamp = atoi(sts);
    
    return true;
}    


void WIFI_Init(void)
{
    ST7735S_DrawString(0,32,"Init ESP32...",WHITE,BLACK);
    if(!ESP_AT_Init())
    {
        ST7735S_DrawString(0,32,"Failed!!!",RED,BLACK);
        while(1);
    }

    /* 恢复出厂默认 + 关闭回显 + 关闭存储 */
    ST7735S_DrawString(0,48,"Reset ESP32...",WHITE,BLACK);
    if(!ESP_AT_Reset())
    {
        ST7735S_DrawString(0,48,"Failed!!!",RED,BLACK);
        while(1);
    }

    ST7735S_DrawString(0,64,"Init WIFI...",WHITE,BLACK);
    if(!ESP_AT_WIFIInit())
    {
        ST7735S_DrawString(0,64,"Failed!!!",RED,BLACK);
        while(1);
    }

    ST7735S_DrawString(0,80,"Connect WIFI...",WHITE,BLACK);
    if(!ESP_AT_WIFIConect(wifi_ssid,wifi_password))
    {
        ST7735S_DrawString(0,80,"Failed!!!",RED,BLACK);
        while(1);
    }

    ST7735S_DrawString(0,96,"Sync Time...",WHITE,BLACK);
    if(!ESP_AT_sntpInit())
    {
        ST7735S_DrawString(0,96,"Failed!!!",RED,BLACK);
        while(1);
    }
}

