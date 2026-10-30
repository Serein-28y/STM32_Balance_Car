#ifndef __HHMPU6050_H
#define __HHMPU6050_H 
void MPU6050_Init(void);
//指定地址写
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
//指定地址读
//读取设备ID
uint8_t MPU6050_GetID(void);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
//读取6轴数据    AX,AY,AZ,GX,GY,GZ(A加速度 G角速度)
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);
#endif
