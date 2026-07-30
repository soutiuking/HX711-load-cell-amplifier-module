# HX711 Load Cell Amplifier Module

> 霁泽入微嵌入式工作室 ｜ 双平台 HX711 驱动与 OLED 显示示例

本项目提供 HX711 24 位 A/D 转换器在 **STM32F1**（HAL 库）和 **ESP32-S3**（ESP-IDF）两个平台上的完整驱动实现，每套固件均包含 HX711 底层驱动和 SSD1306 OLED 显示驱动。

## 核心代码

### STM32F1 版本（`test/Core/`）

| 文件 | 说明 |
| --- | --- |
| `Inc/hx711.h` / `Src/hx711.c` | HX711 驱动：初始化、读取 AD 值、电压换算 |
| `Inc/oled.h` / `Src/oled.c` | SSD1306 OLED 128×32 驱动（软件 I2C） |
| `Inc/delay.h` / `Src/delay.c` | SysTick 微秒延时 |
| `Src/myiic.c` | 软件 I2C 时序实现 |
| `Src/main.c` | 主程序：初始化 HX711 和 OLED，循环显示 AD 值和电压 |

驱动依赖 `Inc/oledfont.h`（ASCII 字库）和 `Inc/bmp.h`（位图数据）。

### ESP32-S3 版本（`test-idf/main/`）

| 文件 | 说明 |
| --- | --- |
| `hx711.c` / `hx711.h` | HX711 驱动：含去皮、标定和重量换算 |
| `filter.c` / `filter.h` | 一阶低通滤波 |
| `oled.c` / `oled.h` | SSD1306 OLED 128×64 驱动（软件 I2C） |
| `main.c` | 主程序：三页显示（系统状态 / AD 值 / 重量），后台采样任务 |

驱动依赖 `oledfont.h`（ASCII 字库）和 `bmp.h`（位图数据）。

## 构建

### STM32F1

1. 打开 STM32CubeMX，新建 STM32F103C8Tx 工程。
2. 配置引脚：
   - **PA0**：GPIO_Input（HX711 DT）
   - **PA1**：GPIO_Output（HX711 SCK）
   - **PA3**：GPIO_Output（OLED SCL）
   - **PA4**：GPIO_Output（OLED SDA）
   - **PA5**：GPIO_Output（OLED RES）
3. 配置 RCC（HSE 或 HSI）和 SYS（Debug Serial Wire）。
4. Project Manager 中选工具链为 MDK-ARM，生成代码。
5. 将本仓库 `test/Core/Inc/` 和 `test/Core/Src/` 中的文件复制到生成工程的对应目录（覆盖 CubeMX 生成的空白包装文件）。
6. 打开 MDK-ARM 工程，编译并烧录。

### ESP32-S3

1. 创建 ESP-IDF 工程：
   ```bash
   idf.py create-project my_hx711
   cd my_hx711
   idf.py set-target esp32s3
   ```
2. 将本仓库 `test-idf/main/` 中所有文件复制到 `main/` 目录。
3. 编辑 `main/CMakeLists.txt`，注册所有源文件：
   ```cmake
   idf_component_register(SRCS "main.c" "hx711.c" "oled.c" "filter.c"
                           INCLUDE_DIRS ".")
   ```
4. 编译并烧录：
   ```bash
   idf.py build
   idf.py -p COM3 flash
   ```

引脚定义见 `hx711.h`（SCK=IO1, DT=IO2）和 `oled.h`（SCL=IO3, SDA=IO4, RES=IO5）。HX711 VCC 接 3.3 V–5 V，GND 共地，称重传感器接 E+/E-（激励）和 A+/A-（信号）。

## 许可证

Apache License 2.0。详见 [LICENSE](LICENSE)。