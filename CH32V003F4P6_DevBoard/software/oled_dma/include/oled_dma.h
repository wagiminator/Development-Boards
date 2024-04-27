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
#include "utils.h"

// I2C Parameters
#define I2C_CLKRATE       400000    // I2C bus clock rate (Hz)
#define I2C_MAP           0         // I2C pin mapping (see above)

// OLED definitions
#define OLED_ADDR         0x78      // OLED write address (0x3C << 1)
#define OLED_CMD_MODE     0x00      // set command mode
#define OLED_DAT_MODE     0x40      // set data mode
#define OLED_WIDTH        128       // OLED width in pixels
#define OLED_HEIGHT       64        // OLED height in pixels

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
