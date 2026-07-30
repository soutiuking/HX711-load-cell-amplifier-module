/**
 *******************************************************************************
 * @file    oled.h
 * @brief   OLED SSD1306 128x32 软件I2C驱动 - HAL版本
 *
 * @version V1.2.0
 * @date    2026-05-02
 *
 * @note    适用于STM32F1系列，使用PA3(SCL)、PA4(SDA)、PA5(RES)
 *******************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#include "main.h"

#define OLED_SCL_PIN   GPIO_PIN_3
#define OLED_SDA_PIN   GPIO_PIN_4
#define OLED_RES_PIN   GPIO_PIN_5
#define OLED_GPIO_PORT GPIOA

#define OLED_CMD  0
#define OLED_DATA 1

#define OLED_SCL_Clr() HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_SCL_PIN, GPIO_PIN_RESET)
#define OLED_SCL_Set() HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_SCL_PIN, GPIO_PIN_SET)

#define OLED_SDA_Clr() HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_SDA_PIN, GPIO_PIN_RESET)
#define OLED_SDA_Set() HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_SDA_PIN, GPIO_PIN_SET)

#define OLED_RES_Clr() HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_RES_PIN, GPIO_PIN_RESET)
#define OLED_RES_Set() HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_RES_PIN, GPIO_PIN_SET)

/**
 * @brief   OLED颜色反转
 * @param   i: 0-正常显示 1-反色显示
 * @retval  None
 */
void OLED_ColorTurn(uint8_t i);

/**
 * @brief   OLED屏幕旋转180度
 * @param   i: 0-正常方向 1-旋转180度
 * @retval  None
 */
void OLED_DisplayTurn(uint8_t i);

/**
 * @brief   I2C起始信号
 * @retval  None
 */
void I2C_Start(void);

/**
 * @brief   I2C停止信号
 * @retval  None
 */
void I2C_Stop(void);

/**
 * @brief   I2C等待ACK响应
 * @retval  None
 */
void I2C_WaitAck(void);

/**
 * @brief   I2C发送一个字节
 * @param   dat: 要发送的数据
 * @retval  None
 */
void Send_Byte(uint8_t dat);

/**
 * @brief   OLED写一个字节
 * @param   dat: 要写入的数据
 * @param   mode: 0-命令模式 1-数据模式
 * @retval  None
 */
void OLED_WR_Byte(uint8_t dat, uint8_t mode);

/**
 * @brief   OLED开启显示
 * @retval  None
 */
void OLED_DisPlay_On(void);

/**
 * @brief   OLED关闭显示
 * @retval  None
 */
void OLED_DisPlay_Off(void);

/**
 * @brief   OLED刷新GRAM到屏幕
 * @note    将显存数据发送到OLED显示，必须在修改GRAM后调用
 * @retval  None
 */
void OLED_Refresh(void);

/**
 * @brief   OLED清屏
 * @note    清除全部GRAM数据并刷新屏幕
 * @retval  None
 */
void OLED_Clear(void);

/**
 * @brief   OLED画点
 * @param   x: 横坐标(0-127)
 * @param   y: 纵坐标(0-63)
 * @param   t: 0-清除点 1-点亮
 * @retval  None
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);

/**
 * @brief   OLED清除点
 * @param   x: 横坐标(0-127)
 * @param   y: 纵坐标(0-63)
 * @retval  None
 */
void OLED_ClearPoint(uint8_t x, uint8_t y);

/**
 * @brief   OLED画线
 * @param   x1: 起点横坐标
 * @param   y1: 起点纵坐标
 * @param   x2: 终点横坐标
 * @param   y2: 终点纵坐标
 * @param   mode: 0-清除线 1-画线
 * @retval  None
 */
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);

/**
 * @brief   OLED画圆
 * @param   x: 圆心横坐标
 * @param   y: 圆心纵坐标
 * @param   r: 圆的半径
 * @retval  None
 */
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r);

/**
 * @brief   OLED显示字符
 * @param   x: 横坐标
 * @param   y: 纵坐标
 * @param   chr: 要显示的字符
 * @param   size1: 字号 8/12/16/24
 * @param   mode: 0-反白显示 1-正常显示
 * @retval  None
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1, uint8_t mode);

/**
 * @brief   OLED显示6x8字符
 * @param   x: 横坐标
 * @param   y: 纵坐标
 * @param   chr: 要显示的字符
 * @param   mode: 0-反白显示 1-正常显示
 * @retval  None
 */
void OLED_ShowChar6x8(uint8_t x, uint8_t y, uint8_t chr, uint8_t mode);

/**
 * @brief   OLED显示字符串
 * @param   x: 横坐标
 * @param   y: 纵坐标
 * @param   chr: 字符串指针
 * @param   size1: 字号
 * @param   mode: 0-反白显示 1-正常显示
 * @retval  None
 */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size1, uint8_t mode);

/**
 * @brief   OLED显示数字
 * @param   x: 横坐标
 * @param   y: 纵坐标
 * @param   num: 数字值
 * @param   len: 数字位数
 * @param   size1: 字号
 * @param   mode: 0-反白显示 1-正常显示
 * @retval  None
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size1, uint8_t mode);

/**
 * @brief   OLED显示汉字（当前为占位空实现）
 * @param   x: 横坐标
 * @param   y: 纵坐标
 * @param   num: 汉字在字库中的索引
 * @param   size1: 字号(仅支持16)
 * @param   mode: 0-反白显示 1-正常显示
 * @note    汉字点阵字库已从开源包中移除，调用后不会显示内容。
 * @retval  None
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1, uint8_t mode);

/**
 * @brief   OLED滚动显示（当前为占位空实现）
 * @param   num: 滚动汉字数量
 * @param   space: 滚动间隔
 * @param   mode: 0-反白显示 1-正常显示
 * @note    汉字点阵字库已从开源包中移除，调用后不会显示内容，也不会进入滚动循环。
 * @retval  None
 */
void OLED_ScrollDisplay(uint8_t num, uint8_t space, uint8_t mode);

/**
 * @brief   OLED显示图片（当前为占位空实现）
 * @param   x: 横坐标
 * @param   y: 纵坐标
 * @param   sizex: 图片宽度
 * @param   sizey: 图片高度
 * @param   BMP: 图片数据数组
 * @param   mode: 0-反白显示 1-正常显示
 * @note    图像数据未随开源包发布，调用后不会显示内容。若要启用图片显示，
 *          请准备 SSD1306 page 格式单色位图：1 字节表示纵向 8 个像素，
 *          bit0 为该页最上方像素；数据按从左到右、从上方页到下方页排列，
 *          字节数为 sizex * ((sizey + 7) / 8)。
 * @retval  None
 */
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, const uint8_t BMP[], uint8_t mode);

/**
 * @brief   OLED停止滚动
 * @retval  None
 */
void OLED_StopScroll(void);

/**
 * @brief   OLED初始化
 * @note    配置GPIO、复位OLED、发送初始化序列
 * @retval  None
 */
void OLED_Init(void);

#endif
