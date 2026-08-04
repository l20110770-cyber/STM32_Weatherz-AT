#ifndef   __SI2C_H
#define   __SI2C_H

void SI2C_W_SCL(uint8_t BitValue);
void SI2C_W_SDA(uint8_t BitValue);
uint8_t SI2C_R_SDA(void);
void SI2C_Init(void);
void SI2C_Start(void);
void SI2C_Stop(void);
void SI2C_SendByte(uint8_t Byte);
uint8_t SI2C_ReceiveByte(void);
void SI2C_SendAck(uint8_t AckBit);
uint8_t SI2C_ReceiveAck(void);

#endif
