#ifndef __MYI2C_H
#define __MYI2C_H

#include <stdint.h>
#include "main.h"

/* ============================================================================
 *  MyI2C —— 软件模拟 I2C 主机（STM32 HAL 版）
 * ----------------------------------------------------------------------------
 *  分层结构：
 *
 *        器件层  MPU6050.c / OLED.c / AT24C02.c ...
 *                     │  只调用下面 7 个函数（不认识 GPIO）
 *                     ▼
 *        协议层  MyI2C_Start / Stop / SendByte / ReceiveByte / SendAck / ReceiveAck
 *                     │  只调用 W_SCL / W_SDA / R_SDA（不认识 MPU6050）
 *                     ▼
 *        引脚层  MyI2C_W_SCL / W_SDA / R_SDA / Init  ← 唯一与芯片库相关
 *
 *  换引脚：只改下面 4 个宏。
 *  换库：  只改 MyI2C.c 里那 4 个引脚层函数，协议层一个字都不用动。
 *
 *  硬件要求：I2C 必须开漏输出 + 上拉电阻。
 *           MPU6050 / OLED 模块板载一般已有 4.7k 上拉，裸芯片必须外接。
 * ==========================================================================*/

/* ---- 引脚配置：换线只改这里 --------------------------------------------- */
#if !defined(MYI2C_SCL_PORT)
    #ifdef hhSCL_Pin                    /* CubeMX 里把该脚打了 hhSCL 标签 */
        #define MYI2C_SCL_PORT      hhSCL_GPIO_Port
        #define MYI2C_SCL_PIN       hhSCL_Pin
    #else                               /* 没有标签，退回江协接法 PB10 */
        #define MYI2C_SCL_PORT      GPIOB
        #define MYI2C_SCL_PIN       GPIO_PIN_10
    #endif
#endif

#if !defined(MYI2C_SDA_PORT)
    #ifdef hhSDA_Pin
        #define MYI2C_SDA_PORT      hhSDA_GPIO_Port
        #define MYI2C_SDA_PIN       hhSDA_Pin
    #else                               /* 没有标签，退回江协接法 PB11 */
        #define MYI2C_SDA_PORT      GPIOB
        #define MYI2C_SDA_PIN       GPIO_PIN_11
    #endif
#endif

/* ---- 速率微调 ----------------------------------------------------------
 *  软件 I2C 没有"波特率寄存器"，速度完全由 CPU 执行速度决定：
 *      MYI2C_DELAY_COUNT = 0  不插延时。适合 8MHz 左右的主频，SCL 约 100kHz 以内。
 *      主频开到 72MHz 后如果通信不稳，改成 1~3 再试（相当于拖慢 SCL）。
 *  改这个值不会改变任何逻辑，只是每个边沿多空转几圈。
 * ------------------------------------------------------------------------ */
#ifndef MYI2C_DELAY_COUNT
#define MYI2C_DELAY_COUNT           0
#endif

/* ---- 协议层接口（器件层只认这 7 个） ------------------------------------ */
void    MyI2C_Init(void);
void    MyI2C_Start(void);
void    MyI2C_Stop(void);
void    MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void    MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);

#endif
