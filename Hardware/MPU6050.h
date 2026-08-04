#ifndef   __MPU6050_H
#define   __MPU6050_H

#define ACC_SENS        16384.0f
#define GYRO_SENS       131.0f

typedef struct
{
    int16_t Temp_Raw;
    
    /*芯片原始16位采样值*/
    int16_t AccX_Raw;
    int16_t AccY_Raw;
    int16_t AccZ_Raw;
    
    int16_t GyroX_Raw;
    int16_t GyroY_Raw;
    int16_t GyroZ_Raw;

    /*换算后的实际物理值*/
    float AccX;
    float AccY;
    float AccZ;
    
    float GyroX;
    float GyroY;
    float GyroZ;
    
    float Temp;
}MPU6050_DataTypeDef;

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(MPU6050_DataTypeDef *Data);

#endif
