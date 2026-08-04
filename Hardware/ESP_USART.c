#include "stm32f10x.h"                  // Device header
#include <stdint.h>
#include <stdbool.h>
#include "ESP_USART.h"
#include <string.h>

static USART_Receive_Callback_t USART_Receive_Callback;

void ESP_USART_Init(void)
{
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA ,ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2,ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init (GPIOA,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init (GPIOA,&GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate=115200;
    USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_Parity=USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl=DISABLE;
    USART_Init (USART2 ,&USART_InitStructure);
    
    USART_ITConfig (USART2,USART_IT_RXNE ,ENABLE);
    
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel =USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
    NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    
    USART_Cmd(USART2,ENABLE);
}


void ESP_USART_W_Data(uint8_t *Data,uint16_t Length)
{
    for(uint16_t i=0;i<Length;i++)
    {
        while(USART_GetFlagStatus (USART2 ,USART_FLAG_TXE)==RESET);
        USART_SendData (USART2 ,Data[i]);
    }
}

void ESP_USART_W_String(const char *Str)
{
    uint16_t Length = strlen(Str);
    ESP_USART_W_Data ((uint8_t *)Str,Length);
}

void ESP_USART_Receive_Register(USART_Receive_Callback_t Callback)
{
    USART_Receive_Callback = Callback;
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus (USART2 ,USART_IT_RXNE)==SET)
    {
        uint8_t Data = USART_ReceiveData (USART2);
        
        if(USART_Receive_Callback)
        USART_Receive_Callback(Data);
    }
}

