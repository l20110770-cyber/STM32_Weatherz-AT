#ifndef   __ESP_USART_H
#define   __ESP_USART_H

#include <stdint.h>

typedef void (*USART_Receive_Callback_t)(uint8_t Data);

void ESP_USART_Init(void);
void ESP_USART_W_Data(uint8_t *Data,uint16_t Length);
void ESP_USART_W_String(const char *Str);
void ESP_USART_Receive_Register(USART_Receive_Callback_t Callback);

#endif
