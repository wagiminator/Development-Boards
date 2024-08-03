// ===================================================================================
// SSD1306 128x64 Pixels I2C OLED Continuous DMA Refresh Functions            * v0.3 *
// ===================================================================================
//
// Functions available:
// --------------------
// OLED_init()                    Init OLED
// OLED_clear(void)               Clear OLED screen
//
// OLED_getPixel(x,y)             Get pixel color at (x,y) (0: pixel cleared, 1: pixel set)
// OLED_setPixel(x,y,c)           Set pixel color (c) at position (x,y)
//
// OLED_drawVLine(x,y,h,c)        Draw vertical line starting from (x,y), height (h), color (c)
// OLED_drawHLine(x,y,w,c)        Draw horizontal line starting from (x,y), width (w), color (c)
// OLED_drawLine(x0,y0,x1,y1,c)   Draw line from position (x0,y0) to (x1,y1) with color (c)
//
// OLED_drawRect(x,y,w,h,c)       Draw rectangle starting from (x,y), width (w), height (h), color (c)
// OLED_fillRect(x,y,w,h,c)       Draw filled rectangle starting from (x,y), width (w), height (h), color (c)
// OLED_drawCircle(x,y,r,c)       Draw circle, center at position (x,y), radius (r), color (c)
// OLED_fillCircle(x,y,r,c)       Draw filled circle, center at position (x,y), radius (r), color (c)
//
// OLED_print(x,y,st,c,sz)        Print string (st) at position (x,y), color (c), size (sz)
// OLED_drawChar(x,y,ch,c,sz)     Draw character (ch) at position (x,y), color (c), size (sz)
// OLED_smoothPrint(x,y,st,c)     Print string (st) at position (x,y), color (c), double-size smoothed
// OLED_smoothChar(x,y,ch,c)      Draw character (ch) at position (x,y), color (c), double-size smoothed
// OLED_stretchPrint(x,y,st,c)    Print string (st) at position (x,y), color (c), v-stretched
// OLED_stretchChar(x,y,ch,c)     Draw character (ch) at position (x,y), color (c), v-stretched
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP    0     1     2
// SDA-pin   PC1   PD0   PC6
// SCL-pin   PC2   PD1   PC5
//
// References:
// -----------
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - David Johnson-Davies:  http://www.technoblogy.com/show?3AJ7
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C Parameters
#define I2C_CLKRATE       400000    // I2C bus clock rate (Hz)
#define I2C_MAP           0         // I2C pin mapping (see above)

// OLED definitions
#define OLED_ADDR         0x78      // OLED write address (0x3C << 1)
#define OLED_CMD_MODE     0x00      // set command mode
#define OLED_DAT_MODE     0x40      // set data mode
#define OLED_WIDTH        128       // OLED width in pixels
#define OLED_HEIGHT       64        // OLED height in pixels

// OLED commands
#define OLED_COLUMN_LOW   0x00      // set lower 4 bits of start column (0x00 - 0x0F)
#define OLED_COLUMN_HIGH  0x10      // set higher 4 bits of start column (0x10 - 0x1F)
#define OLED_MEMORYMODE   0x20      // set memory addressing mode (following byte)
#define OLED_COLUMNS      0x21      // set start and end column (following 2 bytes)
#define OLED_PAGES        0x22      // set start and end page (following 2 bytes)
#define OLED_STARTLINE    0x40      // set display start line (0x40-0x7F = 0-63)
#define OLED_CONTRAST     0x81      // set display contrast (following byte, 0-255)
#define OLED_CHARGEPUMP   0x8D      // (following byte - 0x14:enable, 0x10: disable)
#define OLED_XFLIP_OFF    0xA0      // don't flip display horizontally
#define OLED_XFLIP        0xA1      // flip display horizontally
#define OLED_RESUME       0xA4      // display all on resume
#define OLED_ALL_ON       0xA5      // display all on
#define OLED_INVERT_OFF   0xA6      // set non-inverted display
#define OLED_INVERT       0xA7      // set inverse display
#define OLED_MULTIPLEX    0xA8      // set multiplex ratio (following byte)
#define OLED_DISPLAY_OFF  0xAE      // set display off (sleep mode)
#define OLED_DISPLAY_ON   0xAF      // set display on
#define OLED_PAGE         0xB0      // set start page (0xB0-0xB7 = 0-7)
#define OLED_YFLIP_OFF    0xC0      // don't flip display vertically
#define OLED_YFLIP        0xC8      // flip display vertically
#define OLED_OFFSET       0xD3      // set display offset (y-scroll: following byte)
#define OLED_CLOCK        0xD5      // set frequency (bits 7-4) and divider (bits 3-0)
#define OLED_PRECHARGE    0xD9      // set pre-charge period (following byte)
#define OLED_COMPINS      0xDA      // set COM pin config (following byte)
#define OLED_VCOM_DETECT  0xDB      // set VCOM detect (following byte)

#define OLED_abs(n)       (((n)>=0)?(n):(-(n))) // returns positive value of n

// OLED Functions
void OLED_init(void);
void OLED_clear(void);
uint8_t OLED_getPixel(int16_t x, int16_t y);
void OLED_setPixel(int16_t x, int16_t y, uint8_t color);
void OLED_drawVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
void OLED_drawHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
void OLED_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void OLED_drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void OLED_fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void OLED_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void OLED_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void OLED_drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t size);
void OLED_print(int16_t x, int16_t y, char* str, uint8_t color, uint8_t size);
void OLED_smoothChar(int16_t x, int16_t y, char c, uint8_t color);
void OLED_smoothPrint(int16_t x, int16_t y, char* str, uint8_t color);
void OLED_stretchChar(int16_t x, int16_t y, char c, uint8_t color);
void OLED_stretchPrint(int16_t x, int16_t y, char* str, uint8_t color);

#ifdef __cplusplus
};
#endif
