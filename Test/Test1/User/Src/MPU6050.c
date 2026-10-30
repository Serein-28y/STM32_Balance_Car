#include "MPU6050.h"
#include "MPU6050_Reg.h"
#include "MyI2C.h"

/* ============================================================================
 *  MPU6050 器件层
 * ----------------------------------------------------------------------------
 *  I2C 时序完全交给 MyI2C.c，本文件只做三件事：
 *      1. 把"寄存器地址 + 数据"翻译成 I2C 读写事务
 *      2. 上电时按手册配置几个关键寄存器
 *      3. 把 14 字节原始数据拼成 6 个 int16_t
 * ==========================================================================*/

/**
  * 函    数：MPU6050 写寄存器
  * 参    数：RegAddress  寄存器地址（见 MPU6050_Reg.h）
  * 参    数：Data        要写入的数据，0x00~0xFF
  * 返 回 值：无
  * 说    明：I2C 事务：START → 从机地址(写) → 寄存器地址 → 数据 → STOP
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);        /* 写方向：最低位为 0 */
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(Data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

/**
  * 函    数：MPU6050 读一个寄存器
  * 参    数：RegAddress  寄存器地址
  * 返 回 值：该寄存器的值，0x00~0xFF
  * 说    明：必须先"写"寄存器地址，再用"重复起始"切到读方向。
  *           中间不能 Stop，否则 MPU6050 的内部地址指针会复位。
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);        /* 写方向，目的是送寄存器地址 */
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveAck();

    MyI2C_Start();                          /* 重复起始，不 Stop */
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01); /* 读方向：最低位为 1 */
    MyI2C_ReceiveAck();
    Data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);                       /* 非应答：告诉从机"我只要这一个字节" */
    MyI2C_Stop();

    return Data;
}

/**
  * 函    数：MPU6050 连续读多个寄存器
  * 参    数：RegAddress  起始寄存器地址
  * 参    数：DataArray   输出缓冲区
  * 参    数：Count       要读的字节数
  * 返 回 值：无
  * 说    明：MPU6050 每读一个字节内部地址指针自动 +1，所以能一口气读完。
  *           比"一个寄存器一次事务"快一个数量级。
  *           注意最后一个字节必须回非应答，否则从机会一直占着 SDA。
  */
void MPU6050_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
    uint8_t i;

    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveAck();

    MyI2C_Start();                          /* 重复起始 */
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
    MyI2C_ReceiveAck();

    for (i = 0; i < Count; i ++)
    {
        DataArray[i] = MyI2C_ReceiveByte();
        if (i < Count - 1)
        {
            MyI2C_SendAck(0);               /* 还有后续字节：应答，让从机继续输出 */
        }
        else
        {
            MyI2C_SendAck(1);               /* 最后一个字节：非应答，收回总线控制权 */
        }
    }

    MyI2C_Stop();
}

/**
  * 函    数：MPU6050 初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：只配了 6 个最关键的寄存器，其余保持芯片默认值。
  *           ★ 上电后 PWR_MGMT_1 默认是 0x40（睡眠位 = 1），
  *             必须先写 0x01 唤醒，否则所有数据寄存器都不更新。
  */
void MPU6050_Init(void)
{
    MyI2C_Init();                                   /* 先初始化底层软件 I2C */

    MPU6050_WriteReg(MPU6050_PWR_MGMT_1,  0x01);    /* 退出睡眠；时钟源选 X 轴陀螺仪 PLL */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2,  0x00);    /* 所有轴都不待机 */
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV,  0x07);    /* 采样率 = 1k/(1+7) = 125Hz */
    MPU6050_WriteReg(MPU6050_CONFIG,      0x00);    /* DLPF = 260Hz 带宽，最小延迟 */
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);    /* 陀螺仪 ±2000°/s，16.4 LSB/(°/s) */
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);    /* 加速度计 ±16g，2048 LSB/g */
}

/**
  * 函    数：读取芯片 ID
  * 参    数：无
  * 返 回 值：WHO_AM_I 寄存器的值，正常应为 0x68
  * 说    明：这是验证 I2C 通不通最快的手段。
  *           读不到 0x68 说明接线 / 上拉 / 引脚配置 / 地址有一个错了。
  */
uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

/**
  * 函    数：读取六轴原始数据
  * 参    数：AccX / AccY / AccZ     加速度计三轴，输出参数
  * 参    数：GyroX / GyroY / GyroZ  陀螺仪三轴，输出参数
  * 返 回 值：无
  * 说    明：从 0x3B 开始连续读 14 字节，一次事务搞定：
  *              加速度 6 字节（[0]~[5]）
  *              温度   2 字节（[6]~[7]）—— 这里跳过不用
  *              陀螺仪 6 字节（[8]~[13]）
  *           拼接写法 (Data[0] << 8) | Data[1] 得到的就是二进制补码，
  *           直接赋给 int16_t 会自动解释成正负号，不要再多此一举。
  */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
                     int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
    uint8_t Data[14];

    MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, Data, 14);

    *AccX = (Data[0] << 8) | Data[1];
    *AccY = (Data[2] << 8) | Data[3];
    *AccZ = (Data[4] << 8) | Data[5];
    /* Data[6] / Data[7] 是温度，不用 */
    *GyroX = (Data[8] << 8) | Data[9];
    *GyroY = (Data[10] << 8) | Data[11];
    *GyroZ = (Data[12] << 8) | Data[13];
}
