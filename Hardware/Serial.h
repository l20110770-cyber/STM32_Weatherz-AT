#ifndef   __SERIAL_H
#define   __SERIAL_H

void Serial_Init();
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(char *str);
uint32_t Serial_Pow(uint8_t x,uint8_t y);
void Serial_SendNum(uint32_t Num,uint8_t Length);
void Serial_Printf(char *format,...);
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);
void USART1_IRQHandler(void);

#endif
