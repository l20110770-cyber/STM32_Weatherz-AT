#include "stm32f10x.h"                  // Device header
#include "SI2C.h"
#include "MPU6050_REG.h"
#include "MPU6050.h"

#define MPU6050_ADDRESS     0xD0

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
    SI2C_Start ();
    SI2C_SendByte (MPU6050_ADDRESS);
    SI2C_ReceiveAck ();
    SI2C_SendByte (RegAddress);
    SI2C_ReceiveAck ();
    SI2C_SendByte (Data);
    SI2C_ReceiveAck ();
    SI2C_Stop ();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;
    
    SI2C_Start ();
    SI2C_SendByte (MPU6050_ADDRESS);
    SI2C_ReceiveAck ();
    SI2C_SendByte (RegAddress);
    SI2C_ReceiveAck ();
    
    SI2C_Start ();
    SI2C_SendByte (MPU6050_ADDRESS | 0x01);
    SI2C_ReceiveAck ();
    Data=SI2C_ReceiveByte ();
    SI2C_SendAck (1);
    SI2C_Stop ();
    
    return Data; 
}

void MPU6050_Init(void)
{
    SI2C_Init ();
    
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
    MPU6050_WriteReg(MPU6050_CONFIG,0x06);
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x00);
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x00);
}

uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(MPU6050_DataTypeDef *Data)
{
    uint8_t DataH,DataL;
    
    DataH=MPU6050_ReadReg (MPU6050_ACCEL_XOUT_H );
    DataL=MPU6050_ReadReg (MPU6050_ACCEL_XOUT_L );
    Data->AccX_Raw = (int16_t )(DataH<<8) | DataL;
    Data->AccX = Data->AccX_Raw / ACC_SENS ;
    
    DataH=MPU6050_ReadReg (MPU6050_ACCEL_YOUT_H );
    DataL=MPU6050_ReadReg (MPU6050_ACCEL_YOUT_L );
    Data->AccY_Raw = (int16_t )(DataH<<8) | DataL;
    Data->AccY = Data->AccY_Raw / ACC_SENS;
    
    DataH=MPU6050_ReadReg (MPU6050_ACCEL_ZOUT_H );
    DataL=MPU6050_ReadReg (MPU6050_ACCEL_ZOUT_L );
    Data->AccZ_Raw = (int16_t )(DataH<<8) | DataL;
    Data->AccZ = Data->AccZ_Raw / ACC_SENS;
    
    DataH=MPU6050_ReadReg (MPU6050_GYRO_XOUT_H );
    DataL=MPU6050_ReadReg (MPU6050_GYRO_XOUT_L );
    Data->GyroX_Raw = (int16_t )(DataH<<8) | DataL;
    Data->GyroX = Data->GyroX_Raw / GYRO_SENS;
    
    DataH=MPU6050_ReadReg (MPU6050_GYRO_YOUT_H );
    DataL=MPU6050_ReadReg (MPU6050_GYRO_YOUT_L );
    Data->GyroY_Raw = (int16_t )(DataH<<8) | DataL;
    Data->GyroY = Data->GyroY_Raw / GYRO_SENS;
    
    DataH=MPU6050_ReadReg (MPU6050_GYRO_ZOUT_H );
    DataL=MPU6050_ReadReg (MPU6050_GYRO_ZOUT_L );
    Data->GyroZ_Raw = (int16_t )(DataH<<8) | DataL;
    Data->GyroZ = Data->GyroZ_Raw / GYRO_SENS;
    
    DataH=MPU6050_ReadReg (MPU6050_TEMP_OUT_H );
    DataL=MPU6050_ReadReg (MPU6050_TEMP_OUT_L );
    Data->Temp_Raw = (int16_t )(DataH<<8) | DataL;
    Data->Temp = (Data->Temp_Raw / 340.0f) + 36.53f;
}


