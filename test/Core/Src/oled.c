/**
 * @file    oled.c
 * @brief   OLED SSD1306 I2C驱动 - HAL版本
 * @version v1.0
 * @date    2025-01-01
 */
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"

/* OLED显存GRAM 128x4 共8页
 * SSD1306 128x32 共4页(每页128字节) */
uint8_t OLED_GRAM[128][4];

/**
 * @brief 设置OLED颜色反转
 * @param i 0-正常显示 1-反色显示
 */
void OLED_ColorTurn(uint8_t i)
{
    if (i == 0) {
        OLED_WR_Byte(0xA6, OLED_CMD);
    }
    if (i == 1) {
        OLED_WR_Byte(0xA7, OLED_CMD);
    }
}

/**
 * @brief 设置OLED屏幕旋转
 * @param i 0-正常方向 1-180度旋转
 */
void OLED_DisplayTurn(uint8_t i)
{
    if (i == 0) {
        OLED_WR_Byte(0xC8, OLED_CMD);
        OLED_WR_Byte(0xA1, OLED_CMD);
    }
    if (i == 1) {
        OLED_WR_Byte(0xC0, OLED_CMD);
        OLED_WR_Byte(0xA0, OLED_CMD);
    }
}

/**
 * @brief I2C软件延时
 * @details 使用空循环产生I2C时序所需的延时
 */
void IIC_delay(void)
{
    uint8_t t = 3;
    while (t--);
}

/**
 * @brief I2C起始信号
 * @details SCL高电平时，SDA从高变低
 */
void I2C_Start(void)
{
    OLED_SDA_Set();
    OLED_SCL_Set();
    IIC_delay();
    OLED_SDA_Clr();
    IIC_delay();
    OLED_SCL_Clr();
    IIC_delay();
}

/**
 * @brief I2C停止信号
 * @details SCL高电平时，SDA从低变高
 */
void I2C_Stop(void)
{
    OLED_SDA_Clr();
    OLED_SCL_Set();
    IIC_delay();
    OLED_SDA_Set();
}

/**
 * @brief I2C等待应答信号
 */
void I2C_WaitAck(void)
{
    OLED_SDA_Set();
    IIC_delay();
    OLED_SCL_Set();
    IIC_delay();
    OLED_SCL_Clr();
    IIC_delay();
}

/**
 * @brief I2C发送一个字节数据
 * @param dat 要发送的数据字节
 * @details 高位先出，MSB first
 */
void Send_Byte(uint8_t dat)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        if (dat & 0x80) {
            OLED_SDA_Set();
        } else {
            OLED_SDA_Clr();
        }
        IIC_delay();
        OLED_SCL_Set();
        IIC_delay();
        OLED_SCL_Clr();
        dat <<= 1;
    }
}

/**
 * @brief OLED写一个字节
 * @param dat  要写入的数据
 * @param mode OLED_CMD=命令模式 OLED_DATA=数据模式
 */
void OLED_WR_Byte(uint8_t dat, uint8_t mode)
{
    I2C_Start();
    Send_Byte(0x78);
    I2C_WaitAck();
    if (mode) {
        Send_Byte(0x40);
    } else {
        Send_Byte(0x00);
    }
    I2C_WaitAck();
    Send_Byte(dat);
    I2C_WaitAck();
    I2C_Stop();
}

/**
 * @brief OLED开启显示
 */
void OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_WR_Byte(0xAF, OLED_CMD);
}

/**
 * @brief OLED关闭显示
 */
void OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x10, OLED_CMD);
    OLED_WR_Byte(0xAE, OLED_CMD);
}

/**
 * @brief 刷新GRAM到OLED显示
 * @details 将GRAM中的数据发送到OLED的RAM中，实现显示更新
 */
void OLED_Refresh(void)
{
    uint8_t i, n;
    for (i = 0; i < 4; i++) {
        OLED_WR_Byte(0xB0 + i, OLED_CMD);
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        I2C_Start();
        Send_Byte(0x78);
        I2C_WaitAck();
        Send_Byte(0x40);
        I2C_WaitAck();
        for (n = 0; n < 128; n++) {
            Send_Byte(OLED_GRAM[n][i]);
            I2C_WaitAck();
        }
        I2C_Stop();
    }
}

/**
 * @brief 清空屏幕
 * @details 将GRAM全部置零并刷新屏幕
 */
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 4; i++) {
        for (n = 0; n < 128; n++) {
            OLED_GRAM[n][i] = 0;
        }
    }
    OLED_Refresh();
}

/**
 * @brief 画一个像素点
 * @param x 横坐标 0-127
 * @param y 纵坐标 0-63
 * @param t 1-点亮 0-熄灭
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    if (t) {
        OLED_GRAM[x][i] |= n;
    } else {
        OLED_GRAM[x][i] &= ~n;
    }
}

/**
 * @brief 清除一个像素点
 * @param x 横坐标 0-127
 * @param y 纵坐标 0-63
 */
void OLED_ClearPoint(uint8_t x, uint8_t y)
{
    uint8_t i, m, n;
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    OLED_GRAM[x][i] &= ~n;
}

/**
 * @brief 画一条直线
 * @param x1   起点横坐标
 * @param y1   起点纵坐标
 * @param x2   终点横坐标
 * @param y2   终点纵坐标
 * @param mode 1-画线 0-清除线
 */
void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0) incx = 1;
    else if (delta_x == 0) incx = 0;
    else { incx = -1; delta_x = -delta_x; }
    if (delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;
    else { incy = -1; delta_y = -delta_y; }
    if (delta_x > delta_y) distance = delta_x;
    else distance = delta_y;
    for (t = 0; t < distance + 1; t++) {
        OLED_DrawPoint(uRow, uCol, mode);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief 画一个圆
 * @param x 圆心横坐标
 * @param y 圆心纵坐标
 * @param r 半径
 * @details 使用Bresenham画圆算法
 */
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r)
{
    int a, b, num;
    a = 0;
    b = r;
    while (2 * b * b >= r * r) {
        OLED_DrawPoint(x + a, y - b, 1);
        OLED_DrawPoint(x - a, y - b, 1);
        OLED_DrawPoint(x - a, y + b, 1);
        OLED_DrawPoint(x + a, y + b, 1);
        OLED_DrawPoint(x + b, y + a, 1);
        OLED_DrawPoint(x + b, y - a, 1);
        OLED_DrawPoint(x - b, y - a, 1);
        OLED_DrawPoint(x - b, y + a, 1);
        a++;
        num = (a * a + b * b) - r * r;
        if (num > 0) {
            b--;
            a--;
        }
    }
}

/**
 * @brief 显示一个ASCII字符
 * @param x     横坐标
 * @param y     纵坐标
 * @param chr   字符ASCII码
 * @param size1 字号 8/12/16/24
 * @param mode  1-正常显示 0-反色显示
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size1, uint8_t mode)
{
    uint8_t i, m, temp, size2, chr1;
    uint8_t x0 = x, y0 = y;
    if (size1 == 8) size2 = 6;
    else size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
    chr1 = chr - ' ';
    for (i = 0; i < size2; i++) {
        if (size1 == 8)
            temp = asc2_0806[chr1][i];
        else if (size1 == 12)
            temp = asc2_1206[chr1][i];
        else if (size1 == 16)
            temp = asc2_1608[chr1][i];
        else if (size1 == 24)
            temp = asc2_2412[chr1][i];
        else return;
        for (m = 0; m < 8; m++) {
            if (temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if ((size1 != 8) && ((x - x0) == size1 / 2)) {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示6x8点阵字符(备用显示函数)
 * @param x     横坐标
 * @param y     纵坐标
 * @param chr   字符ASCII码
 * @param mode  1-正常显示 0-反色显示
 */
void OLED_ShowChar6x8(uint8_t x, uint8_t y, uint8_t chr, uint8_t mode)
{
    uint8_t i, m, temp, chr1;
    uint8_t y0 = y;
    chr1 = chr - ' ';
    for (i = 0; i < 6; i++) {
        temp = asc2_0806[chr1][i];
        for (m = 0; m < 8; m++) {
            if (temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        y = y0;
    }
}

/**
 * @brief 显示字符串
 * @param x     横坐标
 * @param y     纵坐标
 * @param chr   字符串指针
 * @param size1 字号 8/12/16/24
 * @param mode  1-正常显示 0-反色显示
 */
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size1, uint8_t mode)
{
    while ((*chr >= ' ') && (*chr <= '~')) {
        OLED_ShowChar(x, y, *chr, size1, mode);
        if (size1 == 8) x += 6;
        else x += size1 / 2;
        chr++;
    }
}

/**
 * @brief 幂运算
 * @param m 底数
 * @param n 指数
 * @return m的n次方
 */
uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) {
        result *= m;
    }
    return result;
}

/**
 * @brief 显示无符号整数
 * @param x     横坐标
 * @param y     纵坐标
 * @param num   要显示的数字
 * @param len   数字位数
 * @param size1 字号 8/12/16/24
 * @param mode  1-正常显示 0-反色显示
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size1, uint8_t mode)
{
    uint8_t t, temp, m = 0;
    if (size1 == 8) m = 2;
    for (t = 0; t < len; t++) {
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        if (temp == 0) {
            OLED_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
        } else {
            OLED_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
        }
    }
}

/**
 * @brief 显示汉字
 * @param x     横坐标
 * @param y     纵坐标
 * @param num   汉字字库索引
 * @param size1 字号 16/24/32/64
 * @param mode  1-正常显示 0-反色显示
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num, uint8_t size1, uint8_t mode)
{
    uint8_t m, temp;
    uint8_t x0 = x, y0 = y;
    uint16_t i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1;
    for (i = 0; i < size3; i++) {
        if (size1 == 16)
            temp = Hzk1[num][i];
        else if (size1 == 24)
            temp = Hzk2[num][i];
        else if (size1 == 32)
            temp = Hzk3[num][i];
        else if (size1 == 64)
            temp = Hzk4[num][i];
        else return;
        for (m = 0; m < 8; m++) {
            if (temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if ((x - x0) == size1) {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 滚动显示汉字
 * @param num   汉字数量
 * @param space 滚动间隔像素数
 * @param mode  1-正常显示 0-反色显示
 * @note 此函数会进入无限循环，需调用OLED_StopScroll停止
 */
void OLED_ScrollDisplay(uint8_t num, uint8_t space, uint8_t mode)
{
    uint8_t i, n, t = 0, m = 0, r;
    while (1) {
        if (m == 0) {
            OLED_ShowChinese(128, 8, t, 16, mode);
            t++;
        }
        if (t == num) {
            for (r = 0; r < 16 * space; r++) {
                for (i = 1; i < 128; i++) {
                    for (n = 0; n < 4; n++) {
                        OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
                    }
                }
                OLED_Refresh();
            }
            t = 0;
        }
        m++;
        if (m == 16) m = 0;
        for (i = 1; i < 128; i++) {
            for (n = 0; n < 4; n++) {
                OLED_GRAM[i - 1][n] = OLED_GRAM[i][n];
            }
        }
        OLED_Refresh();
    }
}

/**
 * @brief 显示图片
 * @param x     横坐标
 * @param y     纵坐标
 * @param sizex 图片宽度
 * @param sizey 图片高度
 * @param BMP   图片数据数组
 * @param mode  1-正常显示 0-反色显示
 */
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, const uint8_t BMP[], uint8_t mode)
{
    uint16_t j = 0;
    uint8_t i, n, temp, m;
    uint8_t x0 = x, y0 = y;
    sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
    for (n = 0; n < sizey; n++) {
        for (i = 0; i < sizex; i++) {
            temp = BMP[j];
            j++;
            for (m = 0; m < 8; m++) {
                if (temp & 0x01) OLED_DrawPoint(x, y, mode);
                else OLED_DrawPoint(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if ((x - x0) == sizex) {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
    }
}

/**
 * @brief 停止滚动显示
 */
void OLED_StopScroll(void)
{
    OLED_WR_Byte(0x2E, 0);
}

/**
 * @brief OLED初始化
 * @details 复位SSD1306并发送初始化命令序列
 */
void OLED_Init(void)
{
    OLED_RES_Clr();
    HAL_Delay(200);
    OLED_RES_Set();

    OLED_WR_Byte(0xAE, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0x10, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xB0, OLED_CMD);
    OLED_WR_Byte(0x81, OLED_CMD);
    OLED_WR_Byte(0xFF, OLED_CMD);
    OLED_WR_Byte(0xA1, OLED_CMD);
    OLED_WR_Byte(0xA6, OLED_CMD);
    OLED_WR_Byte(0xA8, OLED_CMD);
    OLED_WR_Byte(0x1F, OLED_CMD);
    OLED_WR_Byte(0xC8, OLED_CMD);
    OLED_WR_Byte(0xD3, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xD5, OLED_CMD);
    OLED_WR_Byte(0x80, OLED_CMD);
    OLED_WR_Byte(0xD9, OLED_CMD);
    OLED_WR_Byte(0x1F, OLED_CMD);
    OLED_WR_Byte(0xDA, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD);
    OLED_WR_Byte(0x40, OLED_CMD);
    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);
    OLED_Clear();
    OLED_WR_Byte(0xAF, OLED_CMD);
}