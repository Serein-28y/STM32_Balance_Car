#ifndef __SERIAL_H
#define __SERIAL_H
 void InitAllUSART(void);
 void hhSerialSendByte(uint8_t Byte);
 void hhSerialSendArray(uint8_t *Array,uint16_t Length);
 void hhSerialSendString(char *mString);
 void hhSerial_SendNumber(uint32_t Number, uint8_t Length);
 void Serial_Printf(char *format, ...);
 uint8_t Serial_GetRxData(void);
 uint8_t Serial_GetRxFlag(void);



 extern uint8_t BlueSerial_TxPacket[];
  void BlueInitAllUSART(void);
  void hhBlueSerialSendByte(uint8_t Byte);
  void hhBlueSerialSendArray(uint8_t *Array,uint16_t Length);
  void hhBlueSerialSendString(char *mString);
  void hhBlueSerial_SendNumber(uint32_t Number, uint8_t Length);
  void BlueSerial_Printf(char *format, ...);
  uint8_t BlueSerial_GetRxData(void);
  uint8_t BlueSerial_GetRxFlag(void);
  void hhSerial_SendPacket(void);
  extern char BlueSerial_RxPacket[];
 extern uint8_t BlueSerial_RxFlag;
#endif
