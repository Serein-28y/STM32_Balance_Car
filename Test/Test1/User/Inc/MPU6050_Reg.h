#ifndef __MPU6050_REG_H
#define __MPU6050_REG_H

/* ============================================================================
 *  MPU6050 寄存器地址表（对照 MPU6050 数据手册的 Register Map）
 *  这份文件与芯片库无关，标准库 / HAL 版通用，一个字都不用改。
 * ==========================================================================*/

/* ---- 配置类寄存器 ------------------------------------------------------ */
#define MPU6050_SMPLRT_DIV          0x19    /* 采样率分频：采样率 = 1k / (1 + DIV) */
#define MPU6050_CONFIG              0x1A    /* 配置：外部同步 + 数字低通滤波 DLPF */
#define MPU6050_GYRO_CONFIG         0x1B    /* 陀螺仪满量程 */
#define MPU6050_ACCEL_CONFIG        0x1C    /* 加速度计满量程 */

/* ---- 数据寄存器：从 0x3B 开始连续排列，可以一次读完 14 字节 ------------ */
/*     索引      寄存器            内容
 *     [0..1]  0x3B / 0x3C     加速度 X  高 / 低
 *     [2..3]  0x3D / 0x3E     加速度 Y
 *     [4..5]  0x3F / 0x40     加速度 Z
 *     [6..7]  0x41 / 0x42     温度        （一般不用）
 *     [8..9]  0x43 / 0x44     陀螺仪 X
 *     [10..11]0x45 / 0x46     陀螺仪 Y
 *     [12..13]0x47 / 0x48     陀螺仪 Z
 */
#define MPU6050_ACCEL_XOUT_H        0x3B
#define MPU6050_ACCEL_XOUT_L        0x3C
#define MPU6050_ACCEL_YOUT_H        0x3D
#define MPU6050_ACCEL_YOUT_L        0x3E
#define MPU6050_ACCEL_ZOUT_H        0x3F
#define MPU6050_ACCEL_ZOUT_L        0x40
#define MPU6050_TEMP_OUT_H          0x41
#define MPU6050_TEMP_OUT_L          0x42
#define MPU6050_GYRO_XOUT_H         0x43
#define MPU6050_GYRO_XOUT_L         0x44
#define MPU6050_GYRO_YOUT_H         0x45
#define MPU6050_GYRO_YOUT_L         0x46
#define MPU6050_GYRO_ZOUT_H         0x47
#define MPU6050_GYRO_ZOUT_L         0x48

/* ---- 电源管理与身份识别 ------------------------------------------------ */
#define MPU6050_PWR_MGMT_1          0x6B    /* 电源管理 1：睡眠位、时钟源 */
#define MPU6050_PWR_MGMT_2          0x6C    /* 电源管理 2：各轴待机 */
#define MPU6050_WHO_AM_I            0x75    /* 身份寄存器：恒定读出 0x68 */

#endif
