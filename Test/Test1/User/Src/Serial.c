#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
void hhSerialSendByte(uint8_t Byte){
	HAL_UART_Transmit(&huart1, &Byte, 1, HAL_MAX_DELAY);
}
void hhSerialSendArray(uint8_t *Array,uint16_t Length){
	for(uint16_t i=0;i<Length;i++){
		hhSerialSendByte(Array[i]);
	}
}
void hhSerialSendString(char * mString){
	for(uint16_t i=0;mString[i]!='\0';i++){
		hhSerialSendByte(mString[i]);
	}
}
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void hhSerial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		hhSerialSendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}
int fputc(int ch, FILE *f)
{
	hhSerialSendByte(ch);
	return ch;
}
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	hhSerialSendString(String);
}

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;


void InitAllUSART(){
	 HAL_UART_Receive_IT(&huart1, &Serial_RxData, 1);//启动中断接收一个字节
}


uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}



void hhBlueSerialSendByte(uint8_t Byte) {
	//等待数据转移到移位寄存器
	HAL_UART_Transmit(&huart2, &Byte, 1, HAL_MAX_DELAY);
}
void hhBlueSerialSendArray(uint8_t *Array, uint16_t Length) {
	for (uint16_t i = 0; i < Length; i++) {
		hhBlueSerialSendByte(Array[i]);
	}
}
void hhBlueSerialSendString(char *mString) {
	for (uint16_t i = 0; mString[i] != '\0'; i++) {
		hhBlueSerialSendByte(mString[i]);
	}
}
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y) {
	uint32_t Result = 1;
	while (Y--) {
		Result *= X;
	}
	return Result;
}

void hhBlueSerial_SendNumber(uint32_t Number, uint8_t Length) {
	uint8_t i;
	for (i = 0; i < Length; i++) {
		hhBlueSerialSendByte(
				Number / BlueSerial_Pow(10, Length - i - 1) % 10 + '0');
	}
}
void BlueSerial_Printf(char *format, ...) {
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	hhBlueSerialSendString(String);
}

uint8_t BlueSerial_RxData;
uint8_t BlueSerial_RxFlag;
uint8_t BlueSerial_GetRxFlag(void) {
	if (BlueSerial_RxFlag == 1) {
		BlueSerial_RxFlag = 0;
		return 1;
	}
	return 0;
}
uint8_t BlueSerial_GetRxData(void) {
	return BlueSerial_RxData;
}
uint8_t BlueSerial_TxPacket[4];
void hhSerial_SendPacket(void) {
	hhBlueSerialSendByte(0xFF);
	hhBlueSerialSendArray(BlueSerial_TxPacket, 4);
	hhBlueSerialSendByte(0xFE);
}
char BlueSerial_RxPacket[100];


void BlueInitAllUSART(){
	HAL_UART_Receive_IT(&huart2, &BlueSerial_RxData, 1); //接收了一次后需要再次打开接收中断为下次中断接收做准备
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart == &huart1){
		Serial_RxFlag=1;//已接收标志位，说明已经接收完一次
		HAL_UART_Receive_IT(&huart1, &Serial_RxData, 1);//接收了一次后需要再次打开接收中断为下次中断接收做准备
	} else if (huart == &huart2) {
		static uint8_t RxState = 0;//接收状态机，0表示等待接收包头，1表示正在接收数据包
		static uint8_t pRxPacket = 0;
		if (RxState == 0) {
			if (BlueSerial_RxData == '[' && BlueSerial_RxFlag == 0) //把数据读取走后才接收下一个数据包防止粘包问题
			{
				RxState = 1;
				pRxPacket = 0;
			}
		} else if (RxState == 1) {
			if (BlueSerial_RxData == ']') {
				RxState = 0;
				BlueSerial_RxPacket[pRxPacket] = '\0'; //字符串必须带'\0'作为结束标志
				BlueSerial_RxFlag = 1;
			} else {
				BlueSerial_RxPacket[pRxPacket] = BlueSerial_RxData;
				pRxPacket++;
			}
		}
		HAL_UART_Receive_IT(&huart2, &BlueSerial_RxData, 1); //接收了一次后需要再次打开接收中断为下次中断接收做准备
	}
}
