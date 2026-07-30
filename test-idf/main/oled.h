/**
 *******************************************************************************
 * @file    oled.h
 * @brief   OLED SSD1306 128x64 软件I2C驱动 - ESP32版本
 * @version v1.0
 *******************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>
#include <driver/gpio.h>

#define OLED_SCL_PIN   3
#define OLED_SDA_PIN   4
#define OLED_RES_PIN   5

#define OLED_CMD  0
#define OLED_DATA 1

extern uint8_t OLED_GRAM[128][8];

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1, uint8_t mode);
void OLED_ShowString(uint8_t x, uint8_t y, const char *chr, uint8_t size1, uint8_t mode);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size1, uint8_t mode);
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t len, uint8_t size1, uint8_t mode);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat, uint8_t mode);

void OLED_SDA_Set(void);
void OLED_SDA_Clr(void);
void OLED_SCL_Set(void);
void OLED_SCL_Clr(void);
void OLED_RES_Set(void);
void OLED_RES_Clr(void);

#endif
