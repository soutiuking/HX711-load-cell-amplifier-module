# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- 恢复开源发布所需的仓库配置文件 `.editorconfig` 和 `.gitattributes`。
- 恢复 STM32 示例中的 `bmp.h`、原工程遗留 `myiic.c` 和 CubeMX 配置 `test.ioc`。
- 恢复 ESP-IDF 示例的工程级 `CMakeLists.txt`、组件级 `main/CMakeLists.txt` 和 `bmp.h`。
- 更新 README，区分默认构建文件和原工程保留参考文件。
- 清空 OLED 图片数据和汉字点阵字库，仅保留可编译占位符和图片数据格式说明。

### Fixed
- 修正 `.gitignore` 误忽略开源复现入口文件的问题。
- 修复 CHANGELOG 中文乱码，统一为 UTF-8 可读内容。
- 明确 STM32 OLED 默认软件 I2C 已内置在 `oled.c` 中，避免误编译旧版 `myiic.c`。
- 将 STM32 汉字显示、汉字滚动和图片显示接口改为空实现，保证调用不显示内容且不依赖已移除资源。

## [1.2.0] - 2025-01-01

### Added
- 增加 OLED 绘图和显示辅助函数。
- 增加 ESP32-S3 版本的 `HX711_Calibrate()` 标定接口。
- 增加 ESP32-S3 OLED 浮点数显示函数 `OLED_ShowFloat()`。

### Changed
- 调整 OLED 显示页面布局。
- 优化 ESP32-S3 示例的重量换算和滤波流程。

## [1.1.0] - 2025-01-01

### Added
- 增加 ESP32-S3（ESP-IDF）版本。
- 增加一阶低通滤波模块 `filter.c` / `filter.h`。
- 增加去皮接口 `HX711_Tare()`。
- 增加比例系数设置接口 `HX711_Set_Scale()`。
- 增加 OLED 三页显示示例：系统状态、AD 值和重量。
- 增加 ESP32-S3 后台采样任务。

## [1.0.0] - 2025-01-01

### Added
- 增加 STM32F1（HAL 库）版本。
- 增加 HX711 24 位 AD 读取驱动。
- 增加 AD 值到电压的换算接口。
- 增加 SSD1306 OLED 128x32 显示驱动。
- 增加软件 I2C 时序实现。
- 增加 STM32CubeMX 和 Keil MDK 使用说明。
