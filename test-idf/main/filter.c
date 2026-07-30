/**
 * @file    filter.c
 * @brief   一阶低通滤波算法实现
 * @version v1.0
 * @date    2025-01-01
 */
#include "filter.h"

/**
 * @brief 全局滤波器实例
 *
 * 用于HX711称重数据的低通滤波，
 * 由HX711_Init()初始化。
 */
Filter_TypeDef weight_filter;

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
void Filter_Init(Filter_TypeDef *filter, float alpha)
{
    filter->prev_output = 0;
    filter->alpha = alpha;
    filter->initialized = 0;
}

/**
 * @brief 应用一阶低通滤波
 *
 * 一阶低通滤波公式:
 * y(n) = alpha * x(n) + (1 - alpha) * y(n-1)
 *
 * 其中:
 * - x(n) 为当前输入值
 * - y(n) 为当前输出值
 * - y(n-1) 为上一次输出值
 * - alpha 为滤波系数
 *
 * 滤波效果:
 * - alpha = 0.1: 滤波较强，适用于稳定称重
 * - alpha = 0.3: 滤波较弱，适用于快速响应
 *
 * @param filter    滤波器实例指针
 * @param new_value 当前输入值
 * @return float    滤波后的输出值
 */
float Filter_Apply(Filter_TypeDef *filter, float new_value)
{
    float output;

    /* 首次调用，直接返回输入值并初始化 */
    if (!filter->initialized) {
        filter->prev_output = new_value;
        filter->initialized = 1;
        return new_value;
    }

    /* 一阶低通滤波计算 */
    output = filter->alpha * new_value + (1 - filter->alpha) * filter->prev_output;
    filter->prev_output = output;

    return output;
}
