/**
 *******************************************************************************
 * @file    main.c
 * @brief   ESP32-S3 HX711称重传感器示例程序
 * @version v1.0
 *
 * ========================== 硬件配置 ==========================
 *
 * 屏幕: 128x32 OLED
 * HX711模块: SCK=GPIO1, DT=GPIO2
 * OLED: SCL=GPIO3, SDA=GPIO4
 *
 * ========================== 显示页面 ==========================
 *
 * 第1页: 系统状态
 * 第2页: AD原始值 + 电压
 * 第3页: 重量显示
 *
 * 每页2秒循环切换
 *
 *******************************************************************************
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hx711.h"
#include "oled.h"
#include "filter.h"

/** @brief 页面切换时间间隔(毫秒) */
#define PAGE_SWITCH_TIME_MS  2000

/** @brief 当前显示页面索引(0/1/2) */
static uint8_t current_page = 0;

/** @brief 滤波后的重量值(克) */
static float filtered_weight = 0.0f;

/** @brief 最新的原始AD值 */
static uint32_t raw_ad = 0;

/** @brief 滤波后的电压值(毫伏) */
static float filtered_voltage = 0.0f;

/**
 * @brief 采样任务(后台运行)
 *
 * 独立任务，每100ms采样一次HX711数据，
 * 更新原始AD值、滤波重量和滤波电压。
 *
 * @param pvParameters 任务参数(未使用)
 */
static void sample_task(void *pvParameters)
{
    while(1) {
        /* 读取原始AD值 */
        raw_ad = HX711_Read();

        /* 计算净AD值(减去零点偏移) */
        uint32_t net = (raw_ad > g_hx711_offset) ? (raw_ad - g_hx711_offset) : 0;

        /* 将净AD值转换为重量 */
        float weight = (float)net / g_hx711_scale;

        /* 应用一阶低通滤波 */
        filtered_weight = Filter_Apply(&weight_filter, weight);

        /* 计算滤波后的电压(毫伏) */
        filtered_voltage = filtered_weight * 1000.0f;

        /* 延时100ms */
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief 显示第1页 - 系统状态
 *
 * 显示系统名称和传感器型号。
 */
static void display_page1(void)
{
    OLED_Clear();
    OLED_ShowString(24, 0, "ESP32-S3", 12, 1);
    OLED_ShowString(16, 2, "HX711 Scale", 12, 1);
    OLED_Refresh();
}

/**
 * @brief 显示第2页 - AD值和电压
 *
 * 显示最新的原始AD值和滤波后的电压。
 */
static void display_page2(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "AD:", 16, 1);
    OLED_ShowNum(20, 0, raw_ad, 7, 16, 1);
    OLED_ShowString(0, 16, "V:", 16, 1);
    OLED_ShowFloat(12, 16, filtered_voltage, 4, 16, 1);
    OLED_ShowString(60, 16, "mV", 16, 1);
    OLED_Refresh();
}

/**
 * @brief 显示第3页 - 重量显示
 *
 * 显示滤波后的重量值，整数部分4位，小数部分2位。
 */
static void display_page3(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "Weight", 12, 1);

    /* 分离整数和小数部分 */
    uint16_t integer = (uint16_t)filtered_weight;
    uint16_t decimal = (uint16_t)((filtered_weight - integer) * 100);

    /* 显示重量: 整数.小数 单位(g) */
    OLED_ShowNum(16, 2, integer, 4, 16, 1);
    OLED_ShowString(56, 2, ".", 16, 1);
    OLED_ShowNum(64, 2, decimal, 2, 16, 1);
    OLED_ShowString(88, 2, "g", 16, 1);
    OLED_Refresh();
}

/**
 * @brief 主任务入口
 *
 * 程序入口函数:
 * 1. 初始化HX711和OLED
 * 2. 显示系统就绪提示
 * 3. 执行去皮操作
 * 4. 启动后台采样任务
 * 5. 循环显示三页内容
 */
void app_main(void)
{
    printf("ESP32-S3 HX711 Test Starting...\n");

    /* 初始化HX711传感器 */
    HX711_Init();
    printf("HX711 initialized\n");

    /* 初始化OLED显示屏 */
    OLED_Init();
    printf("OLED initialized\n");

    /* 显示系统就绪提示 */
    OLED_ShowString(8, 1, "System", 16, 1);
    OLED_ShowString(8, 17, "Ready!", 16, 1);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    OLED_Refresh();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    /* 执行去皮操作(零点标定) */
    HX711_Tare(10);
    printf("HX711 Tare done\n");

    /* 启动后台采样任务 */
    xTaskCreate(sample_task, "sample", 2048, NULL, 1, NULL);
    printf("Sample task started\n");

    /* 主循环: 轮流显示三页 */
    while(1) {
        switch(current_page) {
            case 0:
                display_page1();
                break;
            case 1:
                display_page2();
                break;
            case 2:
                display_page3();
                break;
        }

        /* 切换到下一页 */
        current_page++;
        if(current_page >= 3) {
            current_page = 0;
        }

        /* 每页停留2秒 */
        vTaskDelay(PAGE_SWITCH_TIME_MS / portTICK_PERIOD_MS);
    }
}
