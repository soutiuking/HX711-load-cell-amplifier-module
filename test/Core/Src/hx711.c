/**
 *******************************************************************************
 * @file    hx711.c
 * @brief   HX711压力传感器驱动实现 - STM32 HAL版本
 * @version v1.0
 *
 * ========================== 驱动说明 ==========================
 *
 * 一、硬件概述
 * ----------
 * HX711是一款专为高精度称重传感器设计的24位A/D转换器芯片。
 * - 24位高精度ADC
 * - 内置可编程放大器，增益可选: 128/64/32
 * - 串行通信接口(SCK/DOUT)
 * - 工作电压: 2.6V ~ 5.5V
 *
 * 二、通信协议
 * ----------
 * 1. DOUT为低电平时表示数据已准备好
 * 2. 产生24个SCK时钟脉冲，逐位读取数据(高位在前)
 * 3. 第25个脉冲设置增益(通道A，增益128)
 * 4. 读取的数据为24位补码格式
 *
 * 三、引脚定义
 * ----------
 * - SCK -> PA1
 * - DT  -> PA0
 *
 * ========================== 修改记录 ==========================
 * v1.0 初始版本，实现基本称重功能
 *******************************************************************************
 */
#include "hx711.h"
#include "delay.h"

// HX711初始化
void HX711_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // SCK 输出
    GPIO_InitStruct.Pin = HX711_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(HX711_SCK_GPIO_Port, &GPIO_InitStruct);

    // DT 输入
    GPIO_InitStruct.Pin = HX711_DT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(HX711_DT_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
}

// 读取24位AD值
uint32_t HX711_Read(void)
{
    uint32_t data = 0;
    uint8_t i;

    while(HAL_GPIO_ReadPin(HX711_DT_GPIO_Port, HX711_DT_Pin) == 1);
    Delay_us(1);

    for(i=0;i<24;i++)
    {
        HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_SET);
        Delay_us(1);
        data <<= 1;
        if(HAL_GPIO_ReadPin(HX711_DT_GPIO_Port, HX711_DT_Pin)) data++;
        HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
        Delay_us(1);
    }

    // 增益128，第25个脉冲
    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_SET);
    Delay_us(1);
    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);

    data ^= 0x800000;
    return data;
}

// 换算成电压（mV）
float HX711_Get_Voltage(void)
{
    uint32_t ad = HX711_Read();
    float voltage = (ad * HX711_COEF) / 1000000.0f;
    return voltage;
}
