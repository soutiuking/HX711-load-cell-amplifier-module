#ifndef __BMP_H
#define __BMP_H

#include <stdint.h>

/*
 * Image bitmap data is intentionally not bundled with the open-source package.
 *
 * To add an image:
 * 1. Convert a monochrome image to SSD1306 page format: 1 byte = 8 vertical
 *    pixels, least-significant bit is the top pixel in that page.
 * 2. Store bytes left-to-right, page-by-page from top to bottom.
 * 3. Replace this placeholder array and call:
 *    OLED_ShowPicture(x, y, width, height, your_bitmap, mode);
 *
 * Required byte count:
 *    width * ((height + 7) / 8)
 */
static const uint8_t BMP1[] = {0x00};

#endif