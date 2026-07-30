# Third Party Notices

This project incorporates components from the following third-party projects:

## CMSIS (Cortex Microcontroller Software Interface Standard)
- Source: https://github.com/ARM-software/CMSIS_5
- License: Apache License 2.0
- Copyright: ARM Limited
- Usage: Core peripheral access headers for ARM Cortex-M processors

## STM32F1 HAL/LL Drivers
- Source: https://github.com/STMicroelectronics/STM32CubeF1
- License: BSD-3-Clause (STM32Cube package) / Apache 2.0 (individual components)
- Copyright: STMicroelectronics
- Usage: Hardware abstraction layer for STM32F1 MCU peripherals

## ESP-IDF (Espressif IoT Development Framework)
- Source: https://github.com/espressif/esp-idf
- License: Apache License 2.0
- Copyright: Espressif Systems (Shanghai) Co., Ltd.
- Usage: Development framework for ESP32-S3 firmware

## SSD1306 OLED Display Driver (Reference Implementation)
- Source: Various open-source implementations
- License: Public Domain / MIT (commonly used)
- Usage: Base reference for software I2C OLED driver

## FreeRTOS
- Source: https://github.com/FreeRTOS/FreeRTOS
- License: MIT License
- Copyright: Amazon.com, Inc. or its affiliates
- Usage: Real-time operating system for ESP-IDF (bundled)

## HX711 Driver Implementation
- Source: This project's original work
- License: Apache License 2.0
- Copyright: 霁泽入微(JiZeRuWei) Embedded Studio
- Usage: HX711 load cell amplifier driver for STM32 and ESP32 platforms

## Font Data (OLED Display)
- Source: Various open-source font data
- License: Public Domain (standard ASCII font bitmaps)
- Usage: 8x6, 12x6, 16x8, 24x12 ASCII character bitmaps and Chinese character bitmaps

Note: The Chinese character bitmap data (Hzk1-Hzk4) included in oledfont.h is derived
from publicly available embedded font resources. Please verify the license terms
if redistributing this font data separately.