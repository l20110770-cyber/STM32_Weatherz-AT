#ifndef __MPU6050_REG_H
#define __MPU6050_REG_H

#define	MPU6050_SMPLRT_DIV		0x19            //采样分频，控制传感器输出数据快慢
#define	MPU6050_CONFIG			0x1A            //数字低通滤波
#define	MPU6050_GYRO_CONFIG		0x1B            //陀螺仪量程设置：±250/500/1000/2000°/s
#define	MPU6050_ACCEL_CONFIG	0x1C            //加速度量程：±2g/4g/8g/16g

#define	MPU6050_ACCEL_XOUT_H	0x3B            //加速度X高8位
#define	MPU6050_ACCEL_XOUT_L	0x3C            //      X低8位
#define	MPU6050_ACCEL_YOUT_H	0x3D            //      Y高8位
#define	MPU6050_ACCEL_YOUT_L	0x3E            //      Y低8位
#define	MPU6050_ACCEL_ZOUT_H	0x3F            //      Z高8位
#define	MPU6050_ACCEL_ZOUT_L	0x40            //      Z低8位
#define	MPU6050_TEMP_OUT_H		0x41            //芯片内置温度：高8位
#define	MPU6050_TEMP_OUT_L		0x42            //              低8位
#define	MPU6050_GYRO_XOUT_H		0x43            //陀螺仪X高8位
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B            //电源管理1：唤醒MPU6050,选择时钟源
#define	MPU6050_PWR_MGMT_2		0x6C            //单独关闭某一轴加速度/陀螺仪省电
#define	MPU6050_WHO_AM_I		0x75            //校验硬件是否正常：标准返回值是0x68



#endif
