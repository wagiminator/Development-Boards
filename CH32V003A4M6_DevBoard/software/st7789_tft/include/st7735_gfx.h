// ===================================================================================
// ST7735/ST7789/ILI9340/ILI9341 Color TFT Graphics Functions                 * v1.4 *
// ===================================================================================
//
// Functions available:
// --------------------
// TFT_init()                     Init TFT
// TFT_power(v)                   Turn display on/off (0: display off, 1: display on)
// TFT_sleep(v)                   Set display sleep mode (0: sleep off, 1: sleep on)
// TFT_invert(v)                  Invert display (0: inverse off, 1: inverse on)
// TFT_resync()                   Resync by toggling CS pin (for non-active control of CS-line mode)
//
// TFT_clear()                    Clear TFT screen
// TFT_fill(c)                    Fill screen with color (c)
// TFT_setPixel(x,y,c)            Set pixel color (c) at position (x,y)
//
// TFT_drawVLine(x,y,h,c)         Draw vertical line starting from (x,y), height (h), color (c)
// TFT_drawHLine(x,y,w,c)         Draw horizontal line starting from (x,y), width (w), color (c)
// TFT_drawLine(x0,y0,x1,y1,c)    Draw line from position (x0,y0) to (x1,y1) with color (c)
//
// TFT_drawRect(x,y,w,h,c)        Draw rectangle starting from (x,y), width (w), height (h), color (c)
// TFT_fillRect(x,y,w,h,c)        Draw filled rectangle starting from (x,y), width (w), height (h), color (c)
// TFT_drawCircle(x,y,r,c)        Draw circle, center at position (x,y), radius (r), color (c)
// TFT_fillCircle(x,y,r,c)        Draw filled circle, center at position (x,y), radius (r), color (c)
//
// TFT_drawBitmap(x,y,w,h,*p)     Draw bitmap at (x,y), width (w), hight (h), pointer to bitmap (*p)
// TFT_drawSprite(x,y,w,h,*p,c)   Draw sprite at (x,y), width (w), hight (h), pointer to bitmap (*p), color (c)
// TFT_printBitmap(w,h,*p)        Print bitmap at cursor position with foreground and background color
//
// TFT_cursor(x,y)                Set text cursor at position (x,y)
// TFT_textcolor(c)               Set text color (c)
// TFT_textbackground(c)          Set text background color (c)
// TFT_textsize(sz)               Set text size (sz)
// TFT_write(c)                   Write character at cursor position or handle control characters
// TFT_print(*str)                Print string (*str) at cursor position and settings
// TFT_printSegment(v,d,l,dp)     Print value (v) at cursor position using defined segment font
//                                with (d) number of digits, (l) leading (0: '0', 1: space) and 
//                                decimal point at position (dp) counted from the right
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// TFT_printf(f, ...)             printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// TFT_printD(n)                  Print decimal value
// TFT_printW(n)                  Print 32-bit hex word value
// TFT_printH(n)                  Print 16-bit hex half-word value
// TFT_printB(n)                  Print  8-bit hex byte value
// TFT_printS(s)                  Print string
// TFT_println(s)                 Print string with newline
// TFT_newline()                  Send newline
//
// TFT parameter settings (needs to be checked):
// ---------------------------------------------
// Display                        DRIVER    WIDTH HEIGHT  XOFF  YOFF  INVERT BGR XORD YORD ROTATE
// Adafruit   1.44" 128x128       ST7735R    128   128      2     1      0    1    0    0    1
// AliExpress 1.44" 128x128       ST7735R    128   128      2     1      0    1    0    0    1
// Adafruit   0.96" 160x80        ST7735     160    80      0    24      0    0    1    0    0
// AliExpress 0.96" 160x80        ST7735     160    80      1    26      1    1    0    1    0
// Adafruit   1.8"  160x128       ST7735R    160   128      0     0      0    1    0    1    0
// AliExpress 1.8"  160x128 red   ST7735R    160   128      0     0      0    1    0    1    0
// AliExpress 1.8"  160x128 blue  ST7735R    160   128      1     2      0    0    1    0    0
// Adafruit   1.14" 240x135       ST7789     240   135     40    52      1    0    1    0    0     
// AliExpress 1.14" 240x135       ST7789     240   135     40    53      1    0    0    1    0
// Adafruit   1.3"  240x240       ST7789     240   240      0    80      1    0    1    1    1
// Adafruit   1.54" 240x240       ST7789     240   240      0    80      1    0    1    1    1
// AliExpress 1.54" 240x240       ST7789     240   240      0    80      1    0    1    1    1
// Adafruit   1.9"  320x170       ST7789     320   170      0    36      1    0    0    1    0
// AliExpress 1.9"  320x170       ST7789     320   170      0    36      1    0    0    1    0
// Adafruit   1.47" 320x172 round ST7789     320   172      0    35      1    0    0    1    0
// AliExpress 1.47" 320x172 round ST7789     320   172      0    35      1    0    0    1    0
// Adafruit   2.0"  320x240       ST7789     320   240      0     0      1    0    1    0    0
// AliExpress 2.0"  320x240       ST7789V    320   240      0     0      1    0    0    1    0
// Adafruit   2.2"  320x240       ILI9340C   320   240      0     0      0    1    1    1    0
// AliExpress 2.4"  320x240       ILI9341    320   240      0     0      0    1    0    0    0
//
// Notes:
// ------
// - Define TFT parameters down below!
// - This library works without a screen buffer.
// - color: 16-bit color mode (5 bits red, 6 bits green, 5 bits blue) or
//          12-bit color mode (4 bits red, 4 bits green, 4 bits blue)
// - size:  1: normal 6x8 pixels, 2: double size (12x16), ... , 8: 8 times (48x64)
//          9: smoothed double size (12x16), 10: v-stretched (6x16)
//
// References:
// -----------
// - David Johnson-Davies:  http://www.technoblogy.com/show?3WAI
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "spi_tx.h"                 // choose your SPI library

// TFT Pins
#define TFT_PIN_DC        PC3       // pin connected to DC (data/command) of TFT
#define TFT_PIN_CS        PC4       // pin connected to CS (select) of TFT

// TFT Hardware Parameters
#define TFT_WIDTH         240       // TFT width in pixels
#define TFT_HEIGHT        135       // TFT height in pixels
#define TFT_XOFF          40        // offset in X-direction
#define TFT_YOFF          53        // offset in Y-direction
#define TFT_INVERT        1         // 1: invert display
#define TFT_BGR           0         // 1: color order blue-green-red
#define TFT_XORDER        0         // 1: flip TFT screen X-direction
#define TFT_YORDER        1         // 1: flip TFT screen Y-direction
#define TFT_ROTATE        0         // 1: rotate TFT screen 90°
#define TFT_COLORBITS     12        // color depth (12 or 16 bits)

// TFT SPI and Timing Parameters
#define TFT_INIT_SPI      1         // 1: init SPI with TFT_init()
#define TFT_CS_CONTROL    0         // 1: active control of CS-line
#define TFT_BOOT_TIME     0         // TFT boot up time in milliseconds
#define TFT_RST_TIME      50        // time to wait after reset in milliseconds
#define TFT_SLPOUT_TIME   150       // time to wait after sleep out in milliseconds

// TFT Text Parameters
#define TFT_PRINT         0         // 1: include print functions (needs print.h)
#define TFT_SEG_FONT      1         // 0: standard font, 1: 13x32 digits, 2: 5x16 digits
#define TFT_SEG_SPACE     3         // width of space between segment digits in pixels
#define TFT_SMOOTH        9         // character size value for double-size smoothed
#define TFT_STRETCH       10        // character size value for v-stretched

// TFT Commands
#define TFT_RESET         0x01      // Software Reset
#define TFT_SLPIN         0x10      // Sleep IN
#define TFT_SLPOUT        0x11      // Sleep Out
#define TFT_PTLON         0x12      // Partial Display Mode On
#define TFT_NORON         0x13      // Normal Display Mode On
#define TFT_INVOFF        0x20      // Display Inversion Off
#define TFT_INVON         0x21      // Display Inversion On
#define TFT_GAMSET        0x26      // Gamma Set
#define TFT_DISPOFF       0x28      // Display Off
#define TFT_DISPON        0x29      // Display On
#define TFT_CASET         0x2A      // Column Address Set
#define TFT_RASET         0x2B      // Row Address Set
#define TFT_RAMWR         0x2C      // Memory Write
#define TFT_RAMRD         0x2E      // Memory Read
#define TFT_PLTAR         0x30      // Partial Area
#define TFT_TEOFF         0x34      // Tearing Effect Line Off
#define TFT_TEON          0x35      // Tearing Effect Line On
#define TFT_MADCTL        0x36      // Memory Data Access Control
#define TFT_IDMOFF        0x38      // Idle Mode Off
#define TFT_IDMON         0x39      // Idle Mode On
#define TFT_COLMOD        0x3A      // Interface Pixel Format

#define TFT_abs(n)        (((n)>=0)?(n):(-(n))) // returns positive value of n

// TFT Control Functions
void TFT_init(void);
void TFT_power(uint8_t yes);
void TFT_sleep(uint8_t yes);
void TFT_invert(uint8_t yes);
void TFT_resync(void);

// TFT Graphics Functions
void TFT_clear(void);
void TFT_fill(uint16_t color);
void TFT_setPixel(int16_t x, int16_t y, uint16_t color);
void TFT_drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void TFT_drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void TFT_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void TFT_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void TFT_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void TFT_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void TFT_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void TFT_drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t* bmp);
void TFT_drawSprite(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp, uint16_t color);
void TFT_printBitmap(uint16_t w, uint16_t h, const uint8_t* bmp);

void TFT_cursor(int16_t x, int16_t y);
void TFT_textcolor(uint16_t color);
void TFT_textbackground(uint16_t color);
void TFT_textsize(uint8_t size);
void TFT_write(char c);
void TFT_print(char* str);

#if TFT_SEG_FONT > 0
void TFT_printSegment(uint16_t value, uint8_t digits, uint8_t lead, uint8_t decimal);
#endif

// TFT Color Calculation
#if TFT_COLORBITS == 16
#define TFT_COLOR(r,g,b)      (((r) & 0xf8) << 8 | ((g) & 0xfc) << 3 | (b) >> 3)      // 16-bit
#else
#define TFT_COLOR(r,g,b)      (((r) & 0xf0) << 8 | ((g) & 0xf0) << 4 | ((b) & 0xf0))  // 12-bit
#endif

// TFT Color Presets
#define TFT_BLACK             TFT_COLOR(  0,   0,   0)
#define TFT_WHITE             TFT_COLOR(255, 255, 255)
#define TFT_RED               TFT_COLOR(255,   0,   0)
#define TFT_GREEN             TFT_COLOR(  0, 255,   0)
#define TFT_BLUE              TFT_COLOR(  0,   0, 255)
#define TFT_YELLOW            TFT_COLOR(255, 255,   0)
#define TFT_CYAN              TFT_COLOR(  0, 255, 255)
#define TFT_MAGENTA           TFT_COLOR(255,   0, 255)

// Additional Print Functions (if activated, see above)
#if TFT_PRINT == 1
#include "print.h"
#define TFT_printD(n)         printD(TFT_write, n)    // print decimal as string
#define TFT_printW(n)         printW(TFT_write, n)    // print word as string
#define TFT_printH(n)         printH(TFT_write, n)    // print half-word as string
#define TFT_printB(n)         printB(TFT_write, n)    // print byte as string
#define TFT_printS(s)         printS(TFT_write, s)    // print string
#define TFT_println(s)        println(TFT_write, s)   // print string with newline
#define TFT_newline()         TFT_write('\n')         // send newline
#define TFT_printf(f, ...)    printF(TFT_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
