# HX711 Load Cell Amplifier Module

> 霁泽入微嵌入式工作室 ｜ 双平台 HX711 驱动与 OLED 显示示例

本项目提供 HX711 24 位 A/D 转换器在 **STM32F1**（HAL 库）和 **ESP32-S3**（ESP-IDF）两个平台上的完整驱动实现，每套固件均包含 HX711 底层驱动和 SSD1306 OLED 显示驱动。

## 核心代码

### STM32F1 版本（`test/Core/`）

| 文件 | 说明 |
| --- | --- |
| `Inc/hx711.h` / `Src/hx711.c` | HX711 驱动：初始化、读取 AD 值、电压换算 |
| `Inc/oled.h` / `Src/oled.c` | SSD1306 OLED 128×32 驱动（内置软件 I2C 时序） |
| `Inc/delay.h` / `Src/delay.c` | SysTick 微秒延时 |
| `Src/main.c` | 主程序：初始化 HX711 和 OLED，循环显示 AD 值和电压 |
| `test.ioc` | STM32CubeMX 工程配置，可用于重新生成 HAL 工程框架 |

驱动依赖 `Inc/oledfont.h`（ASCII 点阵字库）。汉字点阵和图片数据已从开源包中移除，仅保留占位符；`OLED_ShowChinese()`、`OLED_ScrollDisplay()` 和 `OLED_ShowPicture()` 当前不会显示内容。`Inc/bmp.h` 中保留了写入 SSD1306 图片数组的格式说明，`Src/myiic.c` 来自原工程，作为旧版软件 IIC 参考保留，默认示例不需要加入编译。

### ESP32-S3 版本（`test-idf/main/`）

| 文件 | 说明 |
| --- | --- |
| `hx711.c` / `hx711.h` | HX711 驱动：含去皮、标定和重量换算 |
| `filter.c` / `filter.h` | 一阶低通滤波 |
| `oled.c` / `oled.h` | SSD1306 OLED 128×64 驱动（软件 I2C） |
| `main.c` | 主程序：三页显示（系统状态 / AD 值 / 重量），后台采样任务 |
| `CMakeLists.txt` | ESP-IDF 工程和 main 组件构建入口 |

驱动依赖 `oledfont.h`（ASCII 点阵字库）。汉字点阵和图片数据已从开源包中移除，仅保留占位符；`bmp.h` 中保留了写入 SSD1306 图片数组的格式说明，默认示例不需要包含。

## 构建

### STM32F1

1. 使用 STM32CubeMX 打开 `test/test.ioc`，或新建 STM32F103C8Tx 工程后按下列参数配置。
2. 确认引脚：
   - **PA0**：GPIO_Input（HX711 DT）
   - **PA1**：GPIO_Output（HX711 SCK）
   - **PA3**：GPIO_Output（OLED SCL）
   - **PA4**：GPIO_Output（OLED SDA）
   - **PA5**：GPIO_Output（OLED RES）
3. 配置 RCC（HSE 或 HSI）和 SYS（Debug Serial Wire）。
4. Project Manager 中选工具链为 MDK-ARM，生成代码。
5. 将本仓库 `test/Core/Inc/` 和 `test/Core/Src/` 中的核心文件复制到生成工程的对应目录。默认构建需要加入 `delay.c`、`hx711.c`、`oled.c`、`main.c`，不需要加入 `myiic.c`。
6. 打开 MDK-ARM 工程，编译并烧录。

### ESP32-S3

1. 进入 ESP-IDF 示例目录：
   ```bash
   cd test-idf
   idf.py set-target esp32s3
   ```
2. 编译并烧录：
   ```bash
   idf.py build
   idf.py -p COM3 flash
   ```

引脚定义见 `hx711.h`（SCK=IO1, DT=IO2）和 `oled.h`（SCL=IO3, SDA=IO4, RES=IO5）。HX711 VCC 接 3.3 V–5 V，GND 共地，称重传感器接 E+/E-（激励）和 A+/A-（信号）。

## OLED 图片数据格式

本仓库不再内置图片位图数据。需要显示图片时，可在 `bmp.h` 中替换占位数组：

```c
static const uint8_t your_bitmap[] = {
    /* SSD1306 page 格式数据 */
};
```

数据要求：1 字节表示纵向 8 个像素，bit0 为该页最上方像素；按从左到右、从上方页到下方页排列。数组长度应为 `width * ((height + 7) / 8)`。STM32 版本若重新启用显示逻辑，可调用 `OLED_ShowPicture(x, y, width, height, your_bitmap, mode)`。

## 许可证

Apache License 2.0。详见 [LICENSE](LICENSE)。
