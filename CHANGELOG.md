# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- 项目开源发布准备（.gitignore, .editorconfig, .gitattributes）
- Apache 2.0 许可证、贡献指南、行为准则、安全策略
- 中文注释恢复（STM32 OLED驱动）
- 完整 README 文档

### Fixed
- STM32 OLED驱动中文注释乱码修复（850处）
- 移除重复文件和空文本文件

## [1.2.0] - 2025-01-01

### Added
- OLED显示缓存机制防止频闪
- HX711_Calibrate() 标定函数
- 浮点数显示函数 OLED_ShowFloat()

### Changed
- 优化OLED刷新逻辑，只在数值变化时刷新
- 改进ESP32的采样任务稳定性

## [1.1.0] - 2025-01-01

### Added
- ESP32-S3 (ESP-IDF) 固件版本
- 一阶低通滤波算法 (filter.c/h)
- 去皮/零点标定功能 (HX711_Tare)
- 重量比例系数标定 (HX711_Set_Scale)
- OLED 多页显示（系统状态/AD值/重量）
- ESP32 后台采样任务

## [1.0.0] - 2025-01-01

### Added
- STM32F1 (HAL库) 固件版本
- HX711 24位AD值读取
- AD值到电压值转换
- SSD1306 OLED 128x32显示驱动
- 软件I2C通信
- STM32CubeMX 工程文件
- Keil MDK 工程文件