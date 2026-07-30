/**
 * @file    filter.h
 * @brief   一阶低通滤波算法
 * @version v1.0
 * @date    2025-01-01
 */
#ifndef __FILTER_H
#define __FILTER_H

#include "stdint.h"

/**
 * @brief 滤波器数据结构
 *
 * 一阶低通滤波器的状态数据。
 *
 * 成员说明:
 * - prev_output: 上一次滤波输出值
 * - alpha: 滤波系数，范围0~1
 * - initialized: 初始化标志
 */
typedef struct {
    float prev_output;    /**< 上一次滤波输出值 */
    float alpha;          /**< 滤波系数，范围0~1 */
    uint8_t initialized;  /**< 初始化标志 */
} Filter_TypeDef;

/**
 * @brief 全局滤波器实例
 *
 * 用于HX711称重数据的低通滤波。
 * 在filter.c中定义，HX711_Init()中使用。
 */
extern Filter_TypeDef weight_filter;

/**
 * @brief 滤波器初始化
 *
 * 初始化滤波器的参数，设置初始输出为0。
 *
 * @param filter 滤波器实例指针
 * @param alpha  滤波系数，范围0~1
 *               - 值越小滤波越强，响应越慢
 *               - 值越大滤波越弱，响应越快
 */
void Filter_Init(Filter_TypeDef *filter, float alpha);

/**
 * @brief 应用一阶低通滤波
 *
 * 一阶低通滤波公式:
 * y(n) = alpha * x(n) + (1 - alpha) * y(n-1)
 *
 * @param filter    滤波器实例指针
 * @param new_value 当前输入值
 * @return float    滤波后的输出值
 */
float Filter_Apply(Filter_TypeDef *filter, float new_value);

#endif
