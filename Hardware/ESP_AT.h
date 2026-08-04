#ifndef   __ESP_AT_H
#define   __ESP_AT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

extern const char *wifi_ssid;
extern const char *wifi_password;
extern const char *weather_uri;


bool ESP_AT_Init(void);
bool ESP_AT_SendCmd(const char *Cmd,const char **rsp,uint32_t *Length,uint32_t Timeout);
bool ESP_AT_SendData(const uint8_t *Data,uint32_t Length);

bool ESP_AT_Reset(void);

bool ESP_AT_WIFIInit(void);
bool ESP_AT_WIFIConect(const char *ssid,const char *pwd);

bool ESP_AT_httpGet(const char *url,const char **rsp,uint32_t *Length,uint32_t Timeout);
bool ESP_AT_TimeGet(uint32_t *timestamp);

void WIFI_Init(void);
#endif
