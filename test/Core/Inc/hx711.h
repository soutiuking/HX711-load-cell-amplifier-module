#ifndef __HX711_H
#define __HX711_H
/**
 *******************************************************************************
 * @file    hx711.h
 * @brief   HX711压力传感器驱动 - STM32 HAL版本
 * @version v1.0
 *
 * @note    STM32F1平台驱动，使用GPIO模拟时序
 *******************************************************************************
 */
#include "main.h"

// 引脚定义
#define HX711_DT_Pin GPIO_PIN_0
#define HX711_DT_GPIO_Port GPIOA
#define HX711_SCK_Pin GPIO_PIN_1
#define HX711_SCK_GPIO_Port GPIOA

// 3.3V供电，增益128 电压换算系数
#define HX711_COEF 3.07  // 1个AD值 = 3.07nV

void HX711_Init(void);
uint32_t HX711_Read(void);
float HX711_Get_Voltage(void);
#endif
