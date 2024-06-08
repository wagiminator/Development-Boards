// ===================================================================================
// ST7302 250x122 Pixels Monochrome Low-Power LCD Graphics Functions          * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// LCD_init()                     Init LCD
// LCD_sleep(v)                   Set display sleep mode (0: sleep off, 1: sleep on)
// LCD_invert(v)                  Invert display (0: inverse off, 1: inverse on)
//
// LCD_clear()                    Clear LCD screen
// LCD_flush()                    Flush LCD output buffer (call at the end of all drawings)
// LCD_getPixel(x,y)              Get pixel color at (x,y) (0: pixel cleared, 1: pixel set)
// LCD_setPixel(x,y,c)            Set pixel color (c) at position (x,y)
//
// LCD_drawVLine(x,y,h,c)         Draw vertical line starting from (x,y), height (h), color (c)
// LCD_drawHLine(x,y,w,c)         Draw horizontal line starting from (x,y), width (w), color (c)
// LCD_drawLine(x0,y0,x1,y1,c)    Draw line from position (x0,y0) to (x1,y1) with color (c)
//
// LCD_drawRect(x,y,w,h,c)        Draw rectangle starting from (x,y), width (w), height (h), color (c)
// LCD_fillRect(x,y,w,h,c)        Draw filled rectangle starting from (x,y), width (w), height (h), color (c)
// LCD_drawCircle(x,y,r,c)        Draw circle, center at position (x,y), radius (r), color (c)
// LCD_fillCircle(x,y,r,c)        Draw filled circle, center at position (x,y), radius (r), color (c)
//
// LCD_drawBitmap(x,y,w,h,*p)     Draw bitmap at (x,y), width (w), hight (h), pointer to bitmap (*p)
// LCD_drawSprite(x,y,w,h,*p)     Draw sprite at (x,y), width (w), hight (h), pointer to bitmap (*p)
//
// LCD_cursor(x,y)                Set text cursor at position (x,y)
// LCD_textsize(sz)               Set text size (sz)
// LCD_textinvert(v)              Invert text (0: inverse off, 1: inverse on)
// LCD_write(c)                   Write character at cursor position or handle control characters
// LCD_print(str)                 Print string (*str) at cursor position
// LCD_printSegment(v,d,l,dp)     Print value (v) at cursor position using defined segment font
//                                with (d) number of digits, (l) leading (0: '0', 1: space) and 
//                                decimal point at position (dp) counted from the right
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// LCD_cursor(x,y,c,sz)           Set cursor at position (x,y), color (c), size (sz)
// LCD_printf(f, ...)             printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// LCD_printD(n)                  Print decimal value
// LCD_printW(n)                  Print 32-bit hex word value
// LCD_printH(n)                  Print 16-bit hex half-word value
// LCD_printB(n)                  Print  8-bit hex byte value
// LCD_printS(s)                  Print string
// LCD_println(s)                 Print string with newline
// LCD_newline()                  Send newline
//
// Notes:
// ------
// - This library utilizes the integrated software-SPI and works without a screen buffer.
// - color: 0: clear pixel (black), 1: set pixel (white), 2: invert pixel
// - size:  1: normal 6x8 pixels, 2: double size (12x16), ... , 8: 8 times (48x64)
//          9: smoothed double size (12x16), 10: v-stretched (6x16)
//
// References:
// -----------
// - David Johnson-Davies:  http://www.technoblogy.com/show?3YB0
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"

// LCD Pins
#define LCD_PIN_SCL       PC5       // pin connected to SCL (clock) of LCD
#define LCD_PIN_SDA       PC6       // pin connected to SDA (data) of LCD
#define LCD_PIN_DC        PC3       // pin connected to DC (data/command) of LCD
#define LCD_PIN_CS        PC4       // pin connected to CS (select) of LCD

// LCD Parameters
#define LCD_WIDTH         250       // LCD width in pixels
#define LCD_HEIGHT        122       // LCD height in pixels
#define LCD_XOFF          0         // offset in X-direction
#define LCD_YOFF          25        // offset in Y-direction
#define LCD_INVERT        1         // 1: invert display

#define LCD_BOOT_TIME     0         // LCD boot up time in milliseconds
#define LCD_RST_TIME      250       // time to wait after reset in milliseconds
#define LCD_SLPOUT_TIME   100       // time to wait after sleep out in milliseconds

#define LCD_CS_CONTROL    0         // 1: active control of CS-line
#define LCD_FLIP          0         // 1: flip LCD screen
#define LCD_PORTRAIT      0         // 1: use LCD in portrait mode

// LCD Text Settings
#define LCD_PRINT         0         // 1: include print functions (needs print.h)
#define LCD_SEG_FONT      1         // 0: standard font, 1: 13x32 digits, 2: 5x16 digits
#define LCD_SEG_SPACE     3         // width of space between segment digits in pixels
#define LCD_SMOOTH        9         // character size value for double-size smoothed
#define LCD_STRETCH       10        // character size value for v-stretched

// LCD Commands
#define LCD_RESET         0x01      // Software Reset
#define LCD_SLPIN         0x10      // Sleep IN
#define LCD_SLPOUT        0x11      // Sleep Out
#define LCD_INVOFF        0x20      // Display Inversion Off
#define LCD_INVON         0x21      // Display Inversion On
#define LCD_DISPOFF       0x28      // Display Off
#define LCD_DISPON        0x29      // Display On
#define LCD_CASET         0x2A      // Column Address Set
#define LCD_RASET         0x2B      // Row Address Set
#define LCD_RAMWR         0x2C      // Memory Write
#define LCD_RAMRD         0x2E      // Memory Read
#define LCD_MADCTL        0x36      // Memory Data Access Control

#define LCD_abs(n)        (((n)>=0)?(n):(-(n))) // returns positive value of n

// LCD Control Functions
void LCD_init(void);
void LCD_invert(uint8_t yes);
void LCD_sleep(uint8_t yes);

// LCD Graphics Functions
void LCD_clear(void);
void LCD_flush(void);

uint8_t LCD_getPixel(int16_t x, int16_t y);
void LCD_setPixel(int16_t x, int16_t y, uint8_t color);
void LCD_drawVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
void LCD_drawHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void LCD_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void LCD_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);

void LCD_drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp);
void LCD_drawSprite(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp);

void LCD_cursor(int16_t x, int16_t y);
void LCD_textsize(uint8_t size);
void LCD_textinvert(uint8_t yes);
void LCD_write(char c);
void LCD_print(char* str);
void LCD_printSegment(uint16_t value, uint8_t digits, uint8_t lead, uint8_t decimal);

#define LCD_refresh         LCD_flush
#define LCD_textcolor(c)    LCD_textinvert(!(c))

// Additional print functions (if activated, see above)
#if LCD_PRINT == 1
#include "print.h"
#define LCD_printD(n)        printD(LCD_write, n)   // print decimal as string
#define LCD_printW(n)        printW(LCD_write, n)   // print word as string
#define LCD_printH(n)        printH(LCD_write, n)   // print half-word as string
#define LCD_printB(n)        printB(LCD_write, n)   // print byte as string
#define LCD_printS(s)        printS(LCD_write, s)   // print string
#define LCD_println(s)       println(LCD_write, s)  // print string with newline
#define LCD_newline()        LCD_write('\n')        // send newline
#define LCD_printf(f, ...)   printF(LCD_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
