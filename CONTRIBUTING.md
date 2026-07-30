# Contributing to HX711 Load Cell Amplifier Module

欢迎贡献！我们感谢所有形式的贡献——无论是报告问题、提交功能请求、改进文档、还是提交代码。

## 行为准则

本项目采用了 [Contributor Covenant 行为准则](CODE_OF_CONDUCT.md)。请确保所有互动都遵守该准则。

## 如何贡献

### 报告问题

1. 使用 GitHub Issues 报告问题
2. 清晰描述问题的复现步骤
3. 注明使用的硬件平台（STM32F1 / ESP32-S3）
4. 提供相关的代码片段、接线图和错误信息
5. 标注问题的严重程度

### 提交代码

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/your-feature`)
3. 提交更改 (`git commit -am 'Add some feature'`)
4. 推送到分支 (`git push origin feature/your-feature`)
5. 创建 Pull Request

### 编码规范

- C语言遵循 MISRA-C 建议（不完全强制）
- 使用 4 空格缩进
- 函数、变量使用 `snake_case` 命名
- 宏和常量使用 `UPPER_CASE` 命名
- 类型定义使用 `_TypeDef` 后缀
- 文件头部包含 Doxygen 风格的注释块
- 关键函数需添加功能说明和参数注释
- 不修改 STM32CubeMX 自动生成的代码（USER CODE 区域除外）

### 代码审查

所有提交需要经过至少一名维护者的代码审查。审查标准包括：

- 功能正确性
- 代码风格一致性
- 内存和资源使用
- 异常处理
- 注释完整性

### 文档

- 新功能需在 README.md 中添加说明
- API 变更需更新相关文档
- 重要的行为变更需在 CHANGELOG.md 中记录

## 项目结构

本项目包含两个独立的固件工程：

- `test/` - STM32F1 (HAL库) 版本
- `test-idf/` - ESP32-S3 (ESP-IDF) 版本

贡献时应确保：
1. 通用驱动逻辑（如HX711通信时序）在双平台间保持一致
2. 平台特定代码（如延时实现、GPIO配置）与对应硬件适配
3. 新功能至少在一个平台上提供完整实现

## 获取帮助

- 通过 GitHub Issues 提问
- 联系 霁泽入微(JiZeRuWei) Embedded Studio