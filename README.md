# HX711 Load Cell Amplifier Module

> 霁泽入微嵌入式工作室 ｜ 双平台 HX711 驱动与 OLED 显示示例

本项目提供 HX711 24 位 A/D 转换器在 **STM32F1**（HAL 库）和 **ESP32-S3**（ESP-IDF）两个平台上的完整驱动实现，每种固件均包含 HX711 底层驱动和 SSD1306 OLED 显示驱动。

## 核心代码

### STM32F1 版本（`test/`）

| 文件 | 说明 |
| --- | --- |
| `Core/Src/hx711.c` / `Inc/hx711.h` | HX711 驱动：初始化、读取 AD 值、电压换算 |
| `Core/Src/oled.c` / `Inc/oled.h` | SSD1306 OLED 128x32 驱动（软件 I2C） |
| `Core/Src/delay.c` / `Inc/delay.h` | SysTick 微秒延时 |
| `Core/Src/myiic.c` | 软件 I2C 时序实现 |
| `Core/Src/main.c` | 主程序：初始化 HX711 和 OLED，循环显示 AD 值和电压 |

驱动依赖 `Inc/oledfont.h`（ASCII 字库）和 `Inc/bmp.h`（位图数据）。

### ESP32-S3 版本（`test-idf/`）

| 文件 | 说明 |
| --- | --- |
| `main/hx711.c` / `hx711.h` | HX711 驱动：含去皮、标定和重量换算 |
| `main/filter.c` / `filter.h` | 一阶低通滤波 |
| `main/oled.c` / `oled.h` | SSD1306 OLED 128x64 驱动（软件 I2C） |
| `main/main.c` | 主程序：三页显示（系统状态 / AD 值 / 重量），后台采样任务 |

驱动依赖 `main/oledfont.h`（ASCII 字库）和 `main/bmp.h`（位图数据）。

## 构建

### STM32F1 — Keil MDK

打开 `test/MDK-ARM/test.uvprojx`，编译并烧录。引脚配置见 `Inc/hx711.h`（SCK=PA1, DT=PA0）和 `Inc/oled.h`（SCL=PA3, SDA=PA4, RES=PA5）。

### ESP32-S3 — ESP-IDF

```bash
cd test-idf
. $HOME/esp/esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
idf.py -p COM3 flash
```

引脚定义见 `main/hx711.h`（SCK=IO1, DT=IO2）和 `main/oled.h`（SCL=IO3, SDA=IO4, RES=IO5）。

HX711 与 MCU 接线：SCK 接 GPIO 输出，DOUT 接 GPIO 输入，VCC 接 3.3 V-5 V，GND 共地。称重传感器接 HX711 的 E+/E-（激励）和 A+/A-（信号）。

## 许可证

Apache License 2.0。详见 [LICENSE](LICENSE)。