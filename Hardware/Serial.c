#include "stm32f10x.h"                  // Device header

#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void Serial_Init()
{
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 ,ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE );
    
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
    GPIO_Init (GPIOA ,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 ;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
    GPIO_Init (GPIOA ,&GPIO_InitStructure);
    
    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate=9600;
    USART_InitStructure.USART_HardwareFlowControl=DISABLE ;
    USART_InitStructure.USART_Mode =USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStructure.USART_Parity =USART_Parity_No;
    USART_InitStructure.USART_StopBits =USART_StopBits_1;
    USART_InitStructure.USART_WordLength =USART_WordLength_8b;
    USART_Init (USART1 ,&USART_InitStructure);
    
    USART_ITConfig (USART1 ,USART_IT_RXNE,ENABLE );
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE ;
    NVIC_Init (&NVIC_InitStructure);
    
    USART_Cmd (USART1 ,ENABLE );
}

void Serial_SendByte(uint8_t Byte)
{
    USART_SendData (USART1 ,Byte);
    while(USART_GetFlagStatus (USART1,USART_FLAG_TXE )==RESET);
}

void Serial_SendArray(uint8_t *Array,uint16_t Length)
{
    uint16_t i;
    for(i=0;i<Length;i++)
    {
        Serial_SendByte(Array [i]);
    }
}

void Serial_SendString(char *str)
{
    uint8_t i;
    for(i=0;str[i]!='\0';i++)
    {
        Serial_SendByte(str[i]);
    }
}    

uint32_t Serial_Pow(uint8_t x,uint8_t y)
{
    uint32_t Result=1;
    while(y--)
    {
        Result *=x; 
    }
    return Result;
}

void Serial_SendNum(uint32_t Num,uint8_t Length)
{
    uint8_t i;
    for(i=0;i<Length;i++)
    {
    Serial_SendByte(Num/Serial_Pow (10,Length-1-i)%10+'0');
    }
}


void Serial_Printf(char *format,...)
{
    char string[100];
    va_list arg;
    va_start (arg,format);
    vsprintf(string ,format ,arg);
    va_end(arg);
    Serial_SendString(string); 
}

uint8_t Serial_GetRxFlag(void)
{
    if(Serial_RxFlag==1)
    {
        Serial_RxFlag=0;
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t Serial_GetRxData(void)
{
    return Serial_RxData ;
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus (USART1 ,USART_IT_RXNE)==SET)
    {
        Serial_RxData=USART_ReceiveData (USART1 );
        Serial_RxFlag =1;
        USART_ClearITPendingBit (USART1 ,USART_IT_RXNE);
    }
}

