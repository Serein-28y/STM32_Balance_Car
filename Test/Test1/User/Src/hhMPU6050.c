#include "gpio.h"
#include "hhI2C.h" 
#include "hhMPU6050RegAddr.h" 
#define MPU6050_ADDRESS		0xD0  //从机地址 1101 0000
//指定地址写
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();//1.发送起始位
	MyI2C_SendByte(MPU6050_ADDRESS);//2.发送寻找 + 写
	MyI2C_ReceiveAck();//3.接收应答位-这里先不做处理
	MyI2C_SendByte(RegAddress);//4.发送需要写的地址
	MyI2C_ReceiveAck();//5.接收应答位-这里先不做处理
	MyI2C_SendByte(Data);//6.发送需要写的数据
	MyI2C_ReceiveAck();//7.接收应答位-这里先不做处理
	MyI2C_Stop();//8.发送终止条件
}
//指定地址读
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	//1.发送起始位
	MyI2C_Start();
	//2.寻址 + 写
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	//3.发送需写的地址
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	
	//重复起始位
	MyI2C_Start();
	//寻址 + 读
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);//把最后1位改成1，即变成了 1101 0001
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);//发送相应位不应答，从机收到后就不会继续发数据
	//停止
	MyI2C_Stop();
	
	return Data;
}
void MPU6050_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_ReceiveAck();					//接收应答
	
	MyI2C_Start();						//I2C重复起始
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取
	MyI2C_ReceiveAck();					//接收应答
	for (i = 0; i < Count; i ++)		//循环Count次，连续读取多个字节，MPU6050内部地址指针会自动自增
	{
		DataArray[i] = MyI2C_ReceiveByte();	//接收指定寄存器的数据，存入数组第i个数据
		if (i < Count - 1)				//未读取到最后一个字节
		{
			MyI2C_SendAck(0);			//正常发送应答，从机后续会继续输出下一个字节数据
		}
		else							//读取到了最后一个字节
		{
			MyI2C_SendAck(1);			//给非应答，从机后续将不会输出数据，主机收回总线控制权
		}
	}
	MyI2C_Stop();						//I2C终止
}
void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);//解除睡眠模式，并且使用X轴陀螺仪的时钟
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);//所有轴不需要待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);//设置为7分频
	MPU6050_WriteReg(MPU6050_CONFIG, 0x00);//设置低通滤波值
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);//陀螺仪设置量程为最大量程
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);//加速度计设置量程为最大量程
}
//读取设备ID
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
//读取6轴数据
//void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
//						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
//{
//	//由于每个轴的数据都是16位的，分别存放在两个不同的8位的寄存器中
//	uint8_t DataH, DataL;
//	//读取加速度计X轴数据
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);//读取加速度计X轴数据的高8位
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);//读取加速度计X轴数据的低8位
//	*AccX = (DataH << 8) | DataL;//拼接高8位和低8位，组合成一个16位的数据
//	//读取加速度计Y轴数据
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
//	*AccY = (DataH << 8) | DataL;
//	//读取加速度计Z轴数据
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
//	*AccZ = (DataH << 8) | DataL;
//	//读取陀螺仪X轴数据
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
//	*GyroX = (DataH << 8) | DataL;
//	//读取陀螺仪Y轴数据
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
//	*GyroY = (DataH << 8) | DataL;
//	//读取陀螺仪Z轴数据
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
//	*GyroZ = (DataH << 8) | DataL;
//}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t Data[14];
	
	/*通过MPU6050_ReadRegs连续读取多个数据，效率更高*/
	MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, Data, 14);	//从MPU6050_ACCEL_XOUT_H开始，连续读取14个字节，存入Data数组里
	
	/*数据拼接，通过输出参数返回*/
	*AccX = (Data[0] << 8) | Data[1];		//Data[0]和Data[1]为加速度计X轴数据
	*AccY = (Data[2] << 8) | Data[3];		//Data[2]和Data[3]为加速度计Y轴数据
	*AccZ = (Data[4] << 8) | Data[5];		//Data[4]和Data[5]为加速度计Z轴数据
	
											//Data[6]和Data[7]为温度数据，此处暂时不用
	
	*GyroX = (Data[8] << 8) | Data[9];		//Data[8]和Data[9]为陀螺仪X轴数据
	*GyroY = (Data[10] << 8) | Data[11];	//Data[10]和Data[11]为陀螺仪Y轴数据
	*GyroZ = (Data[12] << 8) | Data[13];	//Data[12]和Data[13]为陀螺仪Z轴数据
}





