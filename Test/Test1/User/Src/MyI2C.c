#include "MyI2C.h"

/* ============================================================================
 *  引脚配置层 —— 移植时只需要改这 4 个函数
 *
 *  HAL 版要点：
 *  1. 引脚必须是"开漏输出"(GPIO_MODE_OUTPUT_OD)。开漏才能让从机把线拉低而不烧片子，
 *     写 1 等于"释放总线"，由上拉电阻把线拉到高电平。
 *  2. 开漏输出也能读：HAL_GPIO_ReadPin 读的是 IDR 寄存器，
 *     而 IDR 记录的是引脚的真实电平，跟当前是输入还是输出模式无关。
 *     所以 SDA 不用来回切换输入/输出模式，一根线既能发也能收。
 * ==========================================================================*/

/**
  * 函    数：I2C 写 SCL 引脚电平
  * 参    数：BitValue  要写入 SCL 的电平，0 = 低，1 = 高
  * 返 回 值：无
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
    HAL_GPIO_WritePin(MYI2C_SCL_PORT, MYI2C_SCL_PIN, (GPIO_PinState)BitValue);

#if MYI2C_DELAY_COUNT > 0
    for (volatile uint32_t i = 0; i < MYI2C_DELAY_COUNT; i ++) { __NOP(); }
#endif
}

/**
  * 函    数：I2C 写 SDA 引脚电平
  * 参    数：BitValue  要写入 SDA 的电平，0 = 低，1 = 高
  * 返 回 值：无
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
    HAL_GPIO_WritePin(MYI2C_SDA_PORT, MYI2C_SDA_PIN, (GPIO_PinState)BitValue);

#if MYI2C_DELAY_COUNT > 0
    for (volatile uint32_t i = 0; i < MYI2C_DELAY_COUNT; i ++) { __NOP(); }
#endif
}

/**
  * 函    数：I2C 读 SDA 引脚电平
  * 参    数：无
  * 返 回 值：当前 SDA 电平，低 = 0，高 = 1
  */
uint8_t MyI2C_R_SDA(void)
{
    return (uint8_t)HAL_GPIO_ReadPin(MYI2C_SDA_PORT, MYI2C_SDA_PIN);
}

/**
  * 函    数：I2C 引脚初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：本函数自带 GPIO 时钟使能和引脚配置，因此"放进工程直接调用"即可，
  *           不依赖 CubeMX 是否配过这两个脚（重复配置一遍没有副作用）。
  */
void MyI2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能 SCL / SDA 所在端口的时钟 */
    if (MYI2C_SCL_PORT == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
    else if (MYI2C_SCL_PORT == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
    else if (MYI2C_SCL_PORT == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    else if (MYI2C_SCL_PORT == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
    else if (MYI2C_SCL_PORT == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }

    if (MYI2C_SDA_PORT == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
    else if (MYI2C_SDA_PORT == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
    else if (MYI2C_SDA_PORT == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    else if (MYI2C_SDA_PORT == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
    else if (MYI2C_SDA_PORT == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }

    /* 先把输出寄存器写成 1：这样一旦切换成输出模式，总线立刻是"释放"状态，
       不会在切换瞬间产生一个假的起始信号 */
    HAL_GPIO_WritePin(MYI2C_SCL_PORT, MYI2C_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MYI2C_SDA_PORT, MYI2C_SDA_PIN, GPIO_PIN_SET);

    /* 开漏输出。Pull 在 STM32F1 的输出模式下不生效，这里填 NOPULL 以反映真实情况，
       总线的上拉靠模块板载的 4.7k 电阻 */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = MYI2C_SCL_PIN;
    HAL_GPIO_Init(MYI2C_SCL_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MYI2C_SDA_PIN;
    HAL_GPIO_Init(MYI2C_SDA_PORT, &GPIO_InitStruct);

    /* 初始化完成后总线处于空闲态：SCL = 高，SDA = 高 */
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
}

/* ============================================================================
 *  协议层 —— 与具体芯片库无关，换到任何平台都不用改
 * ==========================================================================*/

/**
  * 函    数：I2C 起始
  * 参    数：无
  * 返 回 值：无
  * 说    明：起始信号 = SCL 高电平期间，SDA 由高变低
  */
void MyI2C_Start(void)
{
    MyI2C_W_SDA(1);                 /* 释放 SDA，确保 SDA 为高电平 */
    MyI2C_W_SCL(1);                 /* 释放 SCL，确保 SCL 为高电平 */
    MyI2C_W_SDA(0);                 /* SCL 高电平期间拉低 SDA，产生起始信号 */
    MyI2C_W_SCL(0);                 /* 起始后把 SCL 也拉低：既占住总线，也方便拼接后续时序 */
}

/**
  * 函    数：I2C 终止
  * 参    数：无
  * 返 回 值：无
  * 说    明：终止信号 = SCL 高电平期间，SDA 由低变高
  */
void MyI2C_Stop(void)
{
    MyI2C_W_SDA(0);                 /* 拉低 SDA，确保 SDA 为低电平 */
    MyI2C_W_SCL(1);                 /* 释放 SCL，使 SCL 呈现高电平 */
    MyI2C_W_SDA(1);                 /* SCL 高电平期间释放 SDA，产生终止信号 */
}

/**
  * 函    数：I2C 发送一个字节
  * 参    数：Byte  要发送的字节，0x00~0xFF
  * 返 回 值：无
  * 说    明：高位先发（MSB First）。SCL 低电平期间改 SDA，SCL 高电平期间从机采样。
  */
void MyI2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i ++)
    {
        /* 两个 ! 把非 0 值统一成 1：!!(0)=0，!!(非0)=1 */
        MyI2C_W_SDA(!!(Byte & (0x80 >> i)));
        MyI2C_W_SCL(1);             /* 释放 SCL，从机在 SCL 高电平期间读取 SDA */
        MyI2C_W_SCL(0);             /* 拉低 SCL，准备发送下一位 */
    }
}

/**
  * 函    数：I2C 接收一个字节
  * 参    数：无
  * 返 回 值：接收到的字节，0x00~0xFF
  * 说    明：主机在 SCL 高电平期间读 SDA，从机在 SCL 低电平期间更新 SDA。
  */
uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0x00;         /* 必须赋初值 0，后面靠"只置 1 不清 0"累加 */
    MyI2C_W_SDA(1);                 /* 接收前先释放 SDA，避免和从机的输出打架 */
    for (i = 0; i < 8; i ++)
    {
        MyI2C_W_SCL(1);             /* 释放 SCL，主机在 SCL 高电平期间读 SDA */
        if (MyI2C_R_SDA()) { Byte |= (0x80 >> i); }
        MyI2C_W_SCL(0);             /* 拉低 SCL，从机在低电平期间写入下一位 */
    }
    return Byte;
}

/**
  * 函    数：I2C 发送应答位
  * 参    数：AckBit  0 = 应答，1 = 非应答
  * 返 回 值：无
  */
void MyI2C_SendAck(uint8_t AckBit)
{
    MyI2C_W_SDA(AckBit);            /* 主机把应答位放到 SDA 上 */
    MyI2C_W_SCL(1);                 /* 释放 SCL，从机在 SCL 高电平期间读取应答位 */
    MyI2C_W_SCL(0);                 /* 拉低 SCL，开始下一个时序模块 */
}

/**
  * 函    数：I2C 接收应答位
  * 参    数：无
  * 返 回 值：0 = 应答（从机拉低了 SDA），1 = 非应答（没人应答）
  * 说    明：MPU6050 的读写法里返回值被丢弃了（江协原版就不判）。
  *           真要判断器件是否在线，用 MPU6050_GetID() 更直接。
  */
uint8_t MyI2C_ReceiveAck(void)
{
    uint8_t AckBit;
    MyI2C_W_SDA(1);                 /* 接收前先释放 SDA，让从机有机会拉低 */
    MyI2C_W_SCL(1);                 /* 释放 SCL，在 SCL 高电平期间读应答位 */
    AckBit = MyI2C_R_SDA();
    MyI2C_W_SCL(0);                 /* 拉低 SCL，开始下一个时序模块 */
    return AckBit;
}
