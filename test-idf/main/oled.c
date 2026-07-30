/**
 *******************************************************************************
 * @file    oled.c
 * @brief   OLED SSD1306 I2C驱动 - ESP32版本
 * @version v1.0
 *
 * ========================== 驱动说明 ==========================
 *
 * 一、硬件概述
 * ----------
 * OLED模块使用SSD1306驱动芯片，支持128x64分辨率。
 *
 * 1. 主要特性:
 *    - 128x64像素分辨率
 *    - I2C通信接口
 *    - 内置GRAM显存
 *    - 支持多种显示功能
 *
 * 2. 引脚定义(ESP32-S3):
 *    - SCL  -> GPIO3
 *    - SDA  -> GPIO4
 *    - RES  -> GPIO5
 *    - VDD  -> 3.3V
 *    - GND  -> GND
 *
 * 3. I2C地址:
 *    - 0x78 (7位地址: 0x3C)
 *
 * ========================== ESP32适配说明 ==========================
 * - 使用软件I2C模拟时序
 * - GPIO驱动实现延时
 * - 引脚定义在oled.h中
 *******************************************************************************
 */

#include "oled.h"
#include "oledfont.h"

#define OLED_ADDR  0x78
#define OLED_CMD   0
#define OLED_DATA  1

/**
 * @brief OLED显存数组
 *
 * 128x64像素分为8页，每页128字节。
 * OLED_GRAM[x][page]存储第x列第page页的数据。
 */
uint8_t OLED_GRAM[128][8];

/**
 * @brief 微秒级延时函数
 *
 * 使用NOP指令循环实现微秒级延时。
 * ESP32-S3主频240MHz下约10次循环1us。
 *
 * @param us 延时时间(微秒)
 */
static void OLED_Delay_us(uint32_t us)
{
    for(uint32_t i = 0; i < us * 10; i++) {
        __asm__ volatile("nop");
    }
}

/**
 * @brief 设置SDA引脚为高电平
 */
void OLED_SDA_Set(void)
{
    gpio_set_level(OLED_SDA_PIN, 1);
}

/**
 * @brief 设置SDA引脚为低电平
 */
void OLED_SDA_Clr(void)
{
    gpio_set_level(OLED_SDA_PIN, 0);
}

/**
 * @brief 设置SCL引脚为高电平
 */
void OLED_SCL_Set(void)
{
    gpio_set_level(OLED_SCL_PIN, 1);
}

/**
 * @brief 设置SCL引脚为低电平
 */
void OLED_SCL_Clr(void)
{
    gpio_set_level(OLED_SCL_PIN, 0);
}

/**
 * @brief 设置RES引脚为高电平
 */
void OLED_RES_Set(void)
{
    gpio_set_level(OLED_RES_PIN, 1);
}

/**
 * @brief 设置RES引脚为低电平
 */
void OLED_RES_Clr(void)
{
    gpio_set_level(OLED_RES_PIN, 0);
}

/**
 * @brief OLED初始化
 *
 * 配置GPIO引脚，执行SSD1306初始化序列。
 * 初始化过程:
 * 1. 配置SCL/SDA/RES为输出
 * 2. 硬件复位OLED
 * 3. 发送初始化命令配置SSD1306
 * 4. 清屏
 */
void OLED_Init(void)
{
    gpio_config_t io_conf = {};

    /* 配置SCL/SDA/RES为输出 */
    io_conf.pin_bit_mask = (1ULL << OLED_SCL_PIN) | (1ULL << OLED_SDA_PIN) | (1ULL << OLED_RES_PIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    /* 硬件复位SSD1306 */
    OLED_RES_Clr();
    OLED_Delay_us(100000);
    OLED_RES_Set();

    /* SSD1306初始化命令序列 */
    OLED_WR_Byte(0xAE, OLED_CMD);  /* 关闭显示 */
    OLED_WR_Byte(0xD5, OLED_CMD);  /* 设置时钟分频 */
    OLED_WR_Byte(0x80, OLED_CMD);
    OLED_WR_Byte(0xA8, OLED_CMD);  /* 设置驱动路数 */
    OLED_WR_Byte(0x3F, OLED_CMD);  /* 1/64 duty */
    OLED_WR_Byte(0xD3, OLED_CMD);  /* 设置显示偏移 */
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0x40, OLED_CMD);  /* 设置显示开始行 */
    OLED_WR_Byte(0x8D, OLED_CMD);  /* 开启电荷泵 */
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_WR_Byte(0x20, OLED_CMD);  /* 设置内存地址模式 */
    OLED_WR_Byte(0x02, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD);  /* 设置段重映射 */
    OLED_WR_Byte(0xC8, OLED_CMD);  /* 设置COM扫描方向 */
    OLED_WR_Byte(0xDA, OLED_CMD);  /* 设置COM引脚配置 */
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0x81, OLED_CMD);  /* 设置对比度 */
    OLED_WR_Byte(0xCF, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD);  /* 设置预充电周期 */
    OLED_WR_Byte(0xF1, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);  /* 设置VCOMH电压 */
    OLED_WR_Byte(0x30, OLED_CMD);
    OLED_WR_Byte(0xA4, OLED_CMD);  /* 整个显示开启 */
    OLED_WR_Byte(0xA6, OLED_CMD);  /* 正常显示 */
    OLED_WR_Byte(0xAF, OLED_CMD);   /* 开启显示 */

    OLED_Clear();
}

/**
 * @brief I2C通信起始信号
 *
 * I2C起始条件: SCL高电平时，SDA从高变低
 */
void I2C_Start(void)
{
    OLED_SDA_Set();
    OLED_SCL_Set();
    OLED_Delay_us(4);
    OLED_SDA_Clr();
    OLED_Delay_us(4);
    OLED_SCL_Clr();
}

/**
 * @brief I2C通信停止信号
 *
 * I2C停止条件: SCL高电平时，SDA从低变高
 */
void I2C_Stop(void)
{
    OLED_SDA_Clr();
    OLED_SCL_Set();
    OLED_Delay_us(4);
    OLED_SDA_Set();
    OLED_Delay_us(4);
}

/**
 * @brief I2C等待从机应答
 */
void I2C_WaitAck(void)
{
    OLED_SDA_Set();
    OLED_Delay_us(2);
    OLED_SCL_Set();
    OLED_Delay_us(2);
    OLED_SCL_Clr();
    OLED_Delay_us(2);
}

/**
 * @brief I2C发送一个字节
 *
 * 高位先出，MSB first
 *
 * @param dat 要发送的数据
 */
void Send_Byte(uint8_t dat)
{
    uint8_t i;
    for(i = 0; i < 8; i++) {
        OLED_SCL_Clr();
        OLED_Delay_us(2);
        if(dat & 0x80) {
            OLED_SDA_Set();
        } else {
            OLED_SDA_Clr();
        }
        dat <<= 1;
        OLED_SCL_Set();
        OLED_Delay_us(2);
    }
    OLED_SCL_Clr();
}

/**
 * @brief 向OLED发送一个字节
 *
 * @param dat 要发送的数据
 * @param mode OLED_CMD=命令, OLED_DATA=数据
 */
void OLED_WR_Byte(uint8_t dat, uint8_t mode)
{
    I2C_Start();
    Send_Byte(OLED_ADDR);         /* 发送OLED I2C地址 */
    I2C_WaitAck();
    if(mode) {
        Send_Byte(0x40);         /* 发送数据标志 */
    } else {
        Send_Byte(0x00);         /* 发送命令标志 */
    }
    I2C_WaitAck();
    Send_Byte(dat);              /* 发送数据 */
    I2C_WaitAck();
    I2C_Stop();
}

/**
 * @brief 刷新OLED显存到屏幕
 *
 * 将OLED_GRAM数组中的数据发送到OLED显示。
 * SSD1306的显存按页组织，需要分8页刷新。
 */
void OLED_Refresh(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++) {
        OLED_WR_Byte(0xB0 + i, OLED_CMD);   /* 设置页地址 */
        OLED_WR_Byte(0x00, OLED_CMD);       /* 设置列低地址 */
        OLED_WR_Byte(0x10, OLED_CMD);       /* 设置列高地址 */
        I2C_Start();
        Send_Byte(OLED_ADDR);
        I2C_WaitAck();
        Send_Byte(0x40);                   /* 开始写数据 */
        I2C_WaitAck();
        for(n = 0; n < 128; n++) {
            Send_Byte(OLED_GRAM[n][i]);
            I2C_WaitAck();
        }
        I2C_Stop();
    }
}

/**
 * @brief 清屏
 *
 * 将OLED_GRAM全部置0，然后刷新显示。
 */
void OLED_Clear(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++) {
        for(n = 0; n < 128; n++) {
            OLED_GRAM[n][i] = 0;
        }
    }
    OLED_Refresh();
}

/**
 * @brief 画点
 *
 * 在指定坐标画一个点。
 *
 * @param x  x坐标(0~127)
 * @param y  y坐标(0~63)
 * @param t  1=点亮, 0=熄灭
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if(t) {
        OLED_GRAM[x][i] |= n;
    } else {
        OLED_GRAM[x][i] &= ~n;
    }
}

/**
 * @brief 显示一个字符
 *
 * 从字库中查找字符点阵并显示。
 * 支持8/12/16/24号字体。
 *
 * @param x      x坐标
 * @param y      y坐标
 * @param chr    字符(ASCII码)
 * @param size1  字体大小(8/12/16/24)
 * @param mode   1=正常显示, 0=反色显示
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1, uint8_t mode)
{
    uint8_t i, m, temp, size2, chr1;
    uint8_t x0 = x, y0 = y;

    /* 根据字体大小计算字符点阵宽度 */
    if(size1 == 8) size2 = 6;
    else size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);

    /* ASCII字符减去空格得到索引 */
    chr1 = chr - ' ';

    /* 逐字节绘制字符点阵 */
    for(i = 0; i < size2; i++) {
        if(size1 == 8) {
            temp = asc2_0806[chr1][i];
        } else if(size1 == 12) {
            temp = asc2_1206[chr1][i];
        } else if(size1 == 16) {
            temp = asc2_1608[chr1][i];
        } else if(size1 == 24) {
            temp = asc2_2412[chr1][i];
        } else return;

        for(m = 0; m < 8; m++) {
            if(temp & 0x01) {
                OLED_DrawPoint(x, y, mode);
            } else {
                OLED_DrawPoint(x, y, !mode);
            }
            temp >>= 1;
            y++;
        }
        x++;
        if((size1 != 8) && ((x - x0) == size1 / 2)) {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示字符串
 *
 * 在指定位置显示一串字符。
 *
 * @param x      x坐标
 * @param y      y坐标
 * @param chr    字符串指针
 * @param size1  字体大小(8/12/16/24)
 * @param mode   1=正常显示, 0=反色显示
 */
void OLED_ShowString(uint8_t x, uint8_t y, const char *chr, uint8_t size1, uint8_t mode)
{
    while((*chr >= ' ') && (*chr <= '~')) {
        OLED_ShowChar(x, y, *chr, size1, mode);
        if(size1 == 8) x += 6;
        else x += size1 / 2;
        chr++;
    }
}

/**
 * @brief 幂运算
 *
 * 计算m的n次方
 *
 * @param m 底数
 * @param n 指数
 * @return uint32_t m^n
 */
uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) {
        result *= m;
    }
    return result;
}

/**
 * @brief 显示数字
 *
 * 在指定位置显示无符号整数。
 *
 * @param x      x坐标
 * @param y      y坐标
 * @param num    要显示的数字
 * @param len    数字位数
 * @param size1  字体大小(8/12/16/24)
 * @param mode   1=正常显示, 0=反色显示
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size1, uint8_t mode)
{
    uint8_t t, i;
    uint8_t enshow = 0;

    for(i = 0; i < len; i++) {
        t = (num / OLED_Pow(10, len - i - 1)) % 10;
        if(enshow == 0 && i < (len - 1)) {
            if(t == 0) {
                OLED_ShowChar(x + (size1 / 2) * i, y, ' ', size1, mode);
                continue;
            } else {
                enshow = 1;
            }
        }
        OLED_ShowChar(x + (size1 / 2) * i, y, t + '0', size1, mode);
    }
}

/**
 * @brief 显示浮点数
 *
 * 在指定位置显示带小数的浮点数。
 *
 * @param x      x坐标
 * @param y      y坐标
 * @param num    要显示的浮点数
 * @param len    整数部分位数
 * @param size1  字体大小(8/12/16/24)
 * @param mode   1=正常显示, 0=反色显示
 */
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t len, uint8_t size1, uint8_t mode)
{
    uint32_t temp;

    /* 处理负数 */
    if(num < 0) {
        OLED_ShowChar(x, y, '-', size1, mode);
        num = -num;
        x += (size1 / 2);
        len--;
    }

    /* 显示整数部分 */
    temp = (uint32_t)num;
    OLED_ShowNum(x, y, temp, len, size1, mode);

    x += (size1 / 2) * len;

    /* 显示小数部分(保留2位) */
    num -= temp;
    temp = (uint32_t)(num * 100);

    OLED_ShowChar(x, y, '.', size1, mode);
    x += (size1 / 2);
    OLED_ShowNum(x, y, temp, 2, size1, mode);
}

/**
 * @brief 设置颜色反转
 *
 * @param i 0=正常颜色, 1=颜色反转
 */
void OLED_ColorTurn(uint8_t i)
{
    if(i == 0) {
        OLED_WR_Byte(0xA6, OLED_CMD);
    }
    if(i == 1) {
        OLED_WR_Byte(0xA7, OLED_CMD);
    }
}

/**
 * @brief 设置屏幕旋转
 *
 * @param i 0=正常显示, 1=旋转180度显示
 */
void OLED_DisplayTurn(uint8_t i)
{
    if(i == 0) {
        OLED_WR_Byte(0xC8, OLED_CMD);
        OLED_WR_Byte(0xA1, OLED_CMD);
    }
    if(i == 1) {
        OLED_WR_Byte(0xC0, OLED_CMD);
        OLED_WR_Byte(0xA0, OLED_CMD);
    }
}
