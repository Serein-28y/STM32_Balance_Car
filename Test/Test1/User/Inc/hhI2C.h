#ifndef __HHI2C_H
#define __HHI2C_H
//初始化GPIO
void MyI2C_Init(void);
//起始或重复起始位
void MyI2C_Start(void);
//发送一个字节
void MyI2C_SendByte(uint8_t Byte);
//接收从机应答位
uint8_t MyI2C_ReceiveAck(void);
//接收一个字节
uint8_t MyI2C_ReceiveByte(void);
//发送应答位
void MyI2C_SendAck(uint8_t AckBit);
//结束条件
void MyI2C_Stop(void);
#endif
