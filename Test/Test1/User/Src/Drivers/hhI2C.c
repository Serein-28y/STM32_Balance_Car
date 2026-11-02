#include "gpio.h"
//对SCL写
//对SCL写   GPIO_PIN_10|GPIO_PIN_11
void MyI2C_W_SCL(GPIO_PinState x)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, x);
//	Delay_us(10);
	//HAL_Delay(1);
}
//对SDA写
void MyI2C_W_SDA(GPIO_PinState x)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, x);
//	Delay_us(10);
	//HAL_Delay(1);
}
//对SDA读
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
//	Delay_us(10);
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)==GPIO_PIN_RESET){
		BitValue=0;
	}else{
		BitValue=1;
	}
//	HAL_Delay(1);
	return BitValue;
}
//初始化GPIO
void MyI2C_Init(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
}
//起始或重复起始位
void MyI2C_Start(void)
{
	//为了兼容重复起始位，这里需先将SDA拉高。假如SCL先拉高，而此时的SDA为低电平(由前时序决定)，将SDA拉高，就会造成SCL高电平期间SDA上升沿。从机会误判为终止条件
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	//起始条件后SCL为低电平
	MyI2C_W_SCL(0);
}
//发送一个字节
void MyI2C_SendByte(uint8_t Byte)
{
	//1.发送前SCL是低电平
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		//2.SCL低电平时SDA放数据
		MyI2C_W_SDA(Byte & (0x80 >> i));//0x80的二进制十1000 0000
		//3.SCL设置高高电平，让从机读取SDA的数据
		MyI2C_W_SCL(1);
		//4.SCL再设置成低电平(所以发完数据后SCL仍然是低电平)
		MyI2C_W_SCL(0);
	}
}
//接收从机应答位
uint8_t MyI2C_ReceiveAck(void)
{
	//1.接收前SLC是低电平
	uint8_t AckBit;
	//2.主机释放SDA，从机检测到后会把数据放在SDA上
	MyI2C_W_SDA(1);
	//3.主机设置SCL为高电平
	MyI2C_W_SCL(1);
	//4.主机读取SDA
	AckBit = MyI2C_R_SDA();
	//5.主机再设置成低电平(所以收完数据后SCL仍然是低电平)
	MyI2C_W_SCL(0);
	return AckBit;
}
//接收一个字节
uint8_t MyI2C_ReceiveByte(void)
{
	//1.接收前SLC是低电平
	uint8_t i, Byte = 0x00;
	//2.主机释放SDA，从机检测到后会把数据放在SDA上
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		//3.主机设置SCL为高电平
		MyI2C_W_SCL(1);
		//4.主机读取SDA
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}
		//5.主机再设置成低电平(所以收完数据后SCL仍然是低电平)
		MyI2C_W_SCL(0);
	}
	return Byte;
}
//发送应答位
void MyI2C_SendAck(uint8_t AckBit)
{
	//1.发送前SCL是低电平
	//2.SCL低电平时SDA放数据
	MyI2C_W_SDA(AckBit);
	//3.SCL设置高高电平，让从机读取SDA的数据
	MyI2C_W_SCL(1);
	//4.SCL再设置成低电平(所以发完数据后SCL仍然是低电平)
	MyI2C_W_SCL(0);
}
//结束条件
void MyI2C_Stop(void)
{
	//SDA在结束前的电平时不确定的，由前时序决定。假如前时序输出的是高电平，那么就做不了结束条件(SCL高电平期间SDA上升沿)。所以这里需要先降SDA拉低
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}
