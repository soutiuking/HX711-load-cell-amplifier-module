/**
 *******************************************************************************
 * @file    hx711.c
 * @brief   HX711压力传感器驱动 - ESP32版本
 * @version v1.2
 *
 * ========================== ESP32适配说明 ==========================
 * - 使用ESP-IDF的GPIO驱动
 * - 使用软件延时实现微秒延时
 * - 引脚定义: SCK=GPIO1, DT=GPIO2
 * - 移植自STM32版本，包含完整的重量计算和低通滤波功能
 *
 * ========================== 驱动说明 ==========================
 *
 * 一、硬件概述
 * ----------
 * HX711是一款专为高精度称重传感器设计的24位A/D转换器芯片。
 *
 * 1. 主要特性:
 *    - 24位高精度ADC (分辨率约为 0.0001%)
 *    - 内置可编程放大器，增益可选: 128/64/32
 *    - 两路差分输入通道(通道A和通道B)
 *    - 串行通信接口(SCK/DOUT)
 *    - 工作电压: 2.6V ~ 5.5V
 *
 * 二、通信协议
 * ----------
 * HX711采用简单的串行协议，遵循以下时序:
 *
 * 1. 数据准备就绪判断:
 *    - DOUT为高电平: A/D转换正在进行，数据未准备好
 *    - DOUT为低电平: 数据已准备好，可以读取
 *
 * 2. 数据读取时序(通道A，增益128):
 *    - ① SCK置低电平
 *    - ② 等待DOUT变为低电平(转换完成)
 *    - ③ 产生24个SCK时钟脉冲，从高到低逐位读取数据
 *    - ④ 第25个脉冲完成数据转换(下降沿触发)
 *    - ⑤ SCK恢复低电平
 *
 * 三、数据格式
 * ----------
 * 1. 输出数据(24位补码格式):
 *    - 范围: 0x000000 ~ 0xFFFFFF
 *    - 有符号范围: -8388608 ~ +8388607
 *    - 零点输出: 约0x800000(中间值)
 *
 * 2. 增益设置(通过脉冲数控制):
 *    - 25个脉冲: 通道A，增益128
 *    - 26个脉冲: 通道B，增益32
 *    - 27个脉冲: 通道A，增益64
 *
 * 四、计算公式
 * ----------
 * 重量计算分两步:
 *
 * 1. AD值转电压:
 *    电压 = (AD值 - 零点) / 4294967 * 激励电压
 *
 * 2. 电压转重量:
 *    重量 = 电压 / (激励电压 * 灵敏度) * 量程
 *
 *    例如: 5Kg传感器，灵敏度1.0mV/V，激励4.3V
 *    重量 = (AD - 零点) / 429.5 (经验公式)
 *
 * 五、标定方法
 * ----------
 * 1. 零点点校正(Tare/去皮):
 *    - 记录无负载时的AD值作为零点偏移
 *    - 实际值 = 测量值 - 零点偏移
 *
 * 2. 比例系数校正:
 *    - 已知重量下测量AD值
 *    - 比例系数 = 净AD值 / 实际重量
 *    - 调整g_hx711_scale直到读数准确
 *
 * ========================== 文件修改记录 ==========================
 * v1.2 2025-01-01 优化滤波算法，添加显示缓存防止频闪
 * v1.1 2025-01-01 添加一阶低通滤波功能
 * v1.0 2025-01-01 初始版本，实现基本称重功能
 *******************************************************************************
 */

#include "hx711.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "oled.h"
#include "filter.h"

#define HX711_SCK_PIN    GPIO_NUM_1
#define HX711_DT_PIN     GPIO_NUM_2

/**
 * @brief 全局零点偏移量
 *
 * 记录无负载时的AD值，用于后续测量时减去零点偏移。
 * 在调用HX711_Tare()后被设置。
 */
int32_t g_hx711_offset;

/**
 * @brief 重量比例系数
 *
 * 用于将净AD值转换为实际重量。
 * 默认值225.0需根据实际传感器标定调整。
 * 计算公式: 重量 = 净AD值 / g_hx711_scale
 */
float g_hx711_scale = 225.f;

/* 上一次显示的原始AD值，用于OLED显示缓存 */
static uint32_t last_raw = 0;
/* 上一次显示的重量值，用于OLED显示缓存 */
static float last_weight = 0;
/* 显示初始化标志，避免首次显示时跳变 */
static uint8_t display_init = 0;

/**
 * @brief 微秒级延时函数
 *
 * 使用NOP指令循环实现微秒级延时，不依赖定时器。
 * 延时精度受CPU主频影响，ESP32-S3主频240MHz下约10次循环1us。
 *
 * @param us 延时时间(微秒)
 */
static void HX711_Delay_us(uint32_t us)
{
    for(uint32_t i = 0; i < us * 10; i++) {
        __asm__ volatile("nop");
    }
}

/**
 * @brief HX711初始化
 *
 * 配置HX711通信引脚:
 * - SCK(GPIO1): 推挽输出，用于发送时钟信号
 * - DT(GPIO2): 上拉输入，用于接收数据
 *
 * 初始化后SCK置高电平(空闲状态)，并初始化低通滤波器。
 */
void HX711_Init(void)
{
    gpio_config_t io_conf = {};

    /* 配置SCK引脚为推挽输出 */
    io_conf.pin_bit_mask = (1ULL << HX711_SCK_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    /* 配置DT引脚为上拉输入 */
    io_conf.pin_bit_mask = (1ULL << HX711_DT_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    /* SCK初始化为高电平(数据手册要求的空闲状态) */
    gpio_set_level(HX711_SCK_PIN, 1);

    /* 初始化一阶低通滤波器，alpha=0.1表示平滑较强 */
    Filter_Init(&weight_filter, 0.1f);
}

/**
 * @brief 从HX711读取24位原始AD值
 *
 * 通信流程:
 * 1. SCK置低，开始通信
 * 2. 等待DT变低，表示转换完成
 * 3. 产生24个时钟脉冲，逐位读取数据(高位在前)
 * 4. 第25个脉冲完成转换，SCK恢复低电平
 *
 * 数据处理:
 * - 读取的24位数据为补码格式
 * - 异或0x800000转换为偏移二进制码
 * - 零点输出约为0x800000(8388608)
 *
 * @return uint32_t 24位AD转换结果
 */
uint32_t HX711_Read(void)
{
    uint8_t i;
    uint32_t count = 0;

    /* SCK置低，开始通信 */
    gpio_set_level(HX711_SCK_PIN, 0);
    count = 0;

    /* 等待DT变低，表示数据转换完成 */
    while (gpio_get_level(HX711_DT_PIN) == 1);

    /* 读取24位数据 */
    for (i = 0; i < 24; i++) {
        /* SCK高电平，数据更新 */
        gpio_set_level(HX711_SCK_PIN, 1);
        count <<= 1;
        HX711_Delay_us(1);

        /* SCK低电平，读取当前位 */
        gpio_set_level(HX711_SCK_PIN, 0);
        if (gpio_get_level(HX711_DT_PIN) == 1) {
            count++;
        }
        HX711_Delay_us(1);
    }

    /* 第25个脉冲，完成转换并进入休眠模式 */
    gpio_set_level(HX711_SCK_PIN, 1);
    count ^= 0x800000;  /* 转换为偏移二进制码 */
    HX711_Delay_us(1);
    gpio_set_level(HX711_SCK_PIN, 0);

    return count;
}

/**
 * @brief 获取当前电压值
 *
 * 读取原始AD值并转换为电压。
 * 电压计算公式: voltage = AD * HX711_COEF / 1000000 (V)
 *
 * @return float 当前电压值(伏特)
 */
float HX711_Get_Voltage(void)
{
    uint32_t ad = HX711_Read();
    float voltage = (ad * HX711_COEF) / 1000000.0f;
    return voltage;
}

/**
 * @brief 去皮(零点标定)
 *
 * 在无负载状态下采集多次AD值，求平均值作为零点偏移。
 * 后续测量值减去此偏移即为实际净重量对应的AD值。
 *
 * 使用场景:
 * - 容器称量时，先放上容器再去皮
 * - 环境温度变化后重新去皮
 * - 长时间运行后去皮修正漂移
 *
 * @param times 采样次数，次数越多越稳定但耗时越长
 * @note 建议采样5~10次
 */
void HX711_Tare(uint8_t times)
{
    uint32_t sum = 0;
    uint8_t i;
    uint32_t val;

    /* 显示去皮状态 */
    OLED_Clear();
    OLED_ShowString(0, 0, "Taring...", 16, 1);
    OLED_Refresh();

    /* 多次采样累加 */
    for (i = 0; i < times; i++) {
        val = HX711_Read();
        sum += val;
    }

    /* 计算零点偏移(平均值) */
    g_hx711_offset = sum / times;

    /* 重置显示状态，强制刷新 */
    display_init = 0;

    /* 重置滤波器，0.3f权重较高使响应更快 */
    Filter_Init(&weight_filter, 0.3f);

    /* 显示标定结果 */
    OLED_Clear();
    OLED_ShowString(0, 0, "Offset:", 16, 1);
    OLED_ShowNum(0, 17, g_hx711_offset, 10, 16, 1);
    OLED_Refresh();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

/**
 * @brief 获取当前重量值
 *
 * 主测量函数，完成以下操作:
 * 1. 多次采样取平均，降低随机噪声
 * 2. 减去零点偏移，得到净AD值
 * 3. 通过比例系数转换为实际重量
 * 4. 经过低通滤波平滑输出
 *
 * 比例换算:
 * 重量 = 净AD值 / g_hx711_scale
 *
 * @param times 采样次数，建议5~10次
 * @return float 当前重量值(克)
 * @note 首次使用需先调用HX711_Tare()进行零点标定
 */
float HX711_GetWeight(uint8_t times)
{
    uint32_t sum = 0;
    uint8_t i;
    uint32_t net;
    float weight;
    uint32_t val;
    uint16_t integer;
    uint16_t decimal;

    /* 多次采样累加 */
    for (i = 0; i < times; i++) {
        val = HX711_Read();
        sum += val;
    }

    /* 取平均值 */
    sum = sum / times;

    /* 计算净AD值(减去零点偏移) */
    if (sum > g_hx711_offset) {
        net = sum - g_hx711_offset;
    } else {
        net = g_hx711_offset - sum;
    }

    /* AD值转换为重量 */
    weight = (float)net / g_hx711_scale;

    /* 一阶低通滤波: y(n) = alpha * x(n) + (1-alpha) * y(n-1) */
    weight = Filter_Apply(&weight_filter, weight);

    /* 分离整数和小数部分 */
    integer = (uint16_t)weight;
    decimal = (uint16_t)((weight - integer) * 100);

    /* 显示缓存机制 - 只在数值变化时刷新OLED防频闪 */
    if (weight != last_weight || sum != last_raw || !display_init) {
        last_weight = weight;
        last_raw = sum;
        display_init = 1;

        OLED_Clear();
        OLED_ShowString(0, 0, "Raw:", 16, 1);
        OLED_ShowNum(30, 0, sum, 10, 16, 1);

        OLED_ShowString(0, 20, "W:", 16, 1);
        OLED_ShowNum(20, 20, integer, 4, 16, 1);
        OLED_ShowString(60, 20, ".", 16, 1);
        OLED_ShowNum(70, 20, decimal, 2, 16, 1);
        OLED_Refresh();
    }

    return weight;
}

/**
 * @brief 获取重量值(简版)
 *
 * 调用HX711_GetWeight(5)的简版包装函数，
 * 固定采样5次取平均。
 *
 * @return int32_t 当前重量值(克)
 */
int32_t HX711_Get_Weight(void)
{
    return (int32_t)HX711_GetWeight(5);
}

/**
 * @brief 设置重量比例系数
 *
 * 比例系数用于将净AD值转换为实际重量。
 * 计算公式: 重量 = 净AD值 / g_hx711_scale
 *
 * @param scale 新的比例系数
 */
void HX711_Set_Scale(float scale)
{
    g_hx711_scale = scale;
}

/**
 * @brief 标定比例系数
 *
 * 使用已知重量进行标定，自动计算比例系数。
 *
 * 标定方法:
 * 1. 确保已完成去皮(HX711_Tare)
 * 2. 放置已知重量的物体
 * 3. 调用本函数，传入已知重量值
 *
 * @param weight 已知重量(克)
 */
void HX711_Calibrate(uint32_t weight)
{
    uint32_t ad = HX711_Read();
    int32_t net = (int32_t)ad - g_hx711_offset;
    if(net != 0) {
        g_hx711_scale = (float)net / (float)weight;
    }
}
