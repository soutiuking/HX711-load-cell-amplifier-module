
/**
 *******************************************************************************
 * @file    myiic.c
 * @brief   软件模拟I2C通信 - STM32 HAL版本
 * @version v1.0
 *
 * @note    使用GPIO模拟I2C主模式时序，适用于OLED等I2C设备通信
 *          引脚定义在myiic.h中
 *******************************************************************************
 */
#include "delay.h"

// 模拟IIC初始化
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = IIC_SCL_Pin|IIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    IIC_SCL_H;
    IIC_SDA_H;
}

// IIC起始信号
void IIC_Start(void)
{
    IIC_SDA_H;
    IIC_SCL_H;
    Delay_us(4);
    IIC_SDA_L;
    Delay_us(4);
    IIC_SCL_L;
}

// IIC停止信号
void IIC_Stop(void)
{
    IIC_SDA_L;
    IIC_SCL_H;
    Delay_us(4);
    IIC_SDA_H;
    Delay_us(4);
}

// IIC写入一个字节
uint8_t IIC_Write_Byte(uint8_t data)
{
    uint8_t i, ack;
    for(i=0;i<8;i++)
    {
        IIC_SCL_L;
        Delay_us(2);
        if(data & 0x80) IIC_SDA_H;
        else IIC_SDA_L;
        data <<= 1;
        IIC_SCL_H;
        Delay_us(2);
    }
    // 等待ACK
    IIC_SCL_L;
    IIC_SDA_H;
    Delay_us(2);
    IIC_SCL_H;
    ack = IIC_READ_SDA;
    IIC_SCL_L;
    return ack;
}
