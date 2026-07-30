/**
 *******************************************************************************
 * @file    hx711.h
 * @brief   HX711压力传感器驱动 - ESP32版本
 * @version v1.2
 *
 * @note    ESP32-S3平台驱动
 *******************************************************************************
 */

#ifndef __HX711_H
#define __HX711_H

#include <stdint.h>

#define HX711_COEF  3.07f

extern int32_t g_hx711_offset;
extern float g_hx711_scale;

void HX711_Init(void);
uint32_t HX711_Read(void);
float HX711_Get_Voltage(void);
int32_t HX711_Get_Weight(void);
float HX711_GetWeight(uint8_t times);
void HX711_Tare(uint8_t times);
void HX711_Set_Scale(float scale);
void HX711_Calibrate(uint32_t weight);

#endif
