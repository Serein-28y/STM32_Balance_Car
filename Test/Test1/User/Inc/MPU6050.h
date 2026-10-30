#ifndef __MPU6050_H
#define __MPU6050_H

#include <stdint.h>

/* ============================================================================
 *  MPU6050 六轴传感器驱动（STM32 HAL 版）
 * ----------------------------------------------------------------------------
 *  依赖：MyI2C.c / MyI2C.h（软件 I2C），以及 MPU6050_Reg.h（寄存器地址表）
 *  用法：
 *      MPU6050_Init();                                  // 上电初始化一次
 *      if (MPU6050_GetID() != 0x68) { 通信失败 }         // 验证 I2C 是否通
 *      MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);        // 主循环里反复读
 *
 *  原始数据单位（量程在 MPU6050_Init 里写死了）：
 *      加速度  满量程 ±16g     → 1 g   = 2048 LSB，  g   = 原始值 / 2048.0
 *      陀螺仪  满量程 ±2000°/s → 1 °/s = 16.4 LSB，  °/s = 原始值 / 16.4
 *      int16_t 有符号，范围 -32768 ~ 32767
 * ==========================================================================*/

/* 从机地址：AD0 接地 = 0xD0（写）/ 0xD1（读），模块默认接地。
   若把模块 AD0 接 3.3V，地址变成 0xD2 / 0xD3。 */
#define MPU6050_ADDRESS             0xD0

void    MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
void    MPU6050_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);

void    MPU6050_Init(void);
uint8_t MPU6050_GetID(void);

void    MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
                        int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);

#endif
