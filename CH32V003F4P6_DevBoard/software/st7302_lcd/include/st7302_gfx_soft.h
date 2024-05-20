// ===================================================================================
// ST7302 250x122 Pixels Monochrome Low-Power LCD Graphics Functions          * v1.0 *
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
// LCD_print(x,y,*st,c,sz)        Print string (*st) at position (x,y), color (c), size (sz)
// LCD_drawChar(x,y,ch,c,sz)      Draw character (ch) at position (x,y), color (c), size (sz)
// LCD_smoothChar(x,y,ch,c)       Draw character (ch) at position (x,y), color (c), double-size smoothed
// LCD_stretchChar(x,y,ch,c)      Draw character (ch) at position (x,y), color (c), v-stretched
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
// - This library utilizes the integrated software-SPI.
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
#define LCD_BOOT_TIME     0         // LCD boot up time in milliseconds
#define LCD_FLIP          0         // 1: flip LCD screen
#define LCD_PORTRAIT      0         // 1: use LCD in portrait mode
#define LCD_PRINT         0         // 1: include print functions (needs print.h)

// Segment Digit Parameters
#define LCD_SEG_FONT      1         // 0: unused, 1: 13x32 digits, 2: 5x16 digits
#define LCD_SEG_SPACE     3         // width of space between segment digits in pixels

// LCD Definitions
#define LCD_SMOOTH        9         // character size value for double-size smoothed
#define LCD_STRETCH       10        // character size value for v-stretched

// LCD Commands
#define LCD_CASET         0x2A      // define column address
#define LCD_RASET         0x2B      // define row address
#define LCD_RAMWR         0x2C      // write to display RAM
#define LCD_RAMRD         0x2E      // read from display RAM

#define LCD_abs(n)        (((n)>=0)?(n):(-(n))) // returns positive value of n

// LCD Control Functions
void LCD_init(void);
void LCD_invert(uint8_t yes);
void LCD_sleep(uint8_t yes);

// LCD Graphics Functions
void LCD_clear(void);
uint8_t LCD_getPixel(int16_t x, int16_t y);
void LCD_setPixel(int16_t x, int16_t y, uint8_t color);
void LCD_flush(void);
void LCD_drawVLine(int16_t x, int16_t y, int16_t h, uint8_t color);
void LCD_drawHLine(int16_t x, int16_t y, int16_t w, uint8_t color);
void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void LCD_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void LCD_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void LCD_drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp);
void LCD_drawSprite(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp);
void LCD_print(int16_t x, int16_t y, char* str, uint8_t color, uint8_t size);
void LCD_drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t size);
void LCD_smoothChar(int16_t x, int16_t y, char c, uint8_t color);
void LCD_stretchChar(int16_t x, int16_t y, char c, uint8_t color);

#if LCD_SEG_FONT > 0
void LCD_printSegment(int16_t x, int16_t y, uint16_t value, uint8_t digits, uint8_t lead, uint8_t decimal);
#endif

// Additional print functions (if activated, see above)
#if LCD_PRINT == 1
#include "print.h"
void LCD_cursor(int16_t x, int16_t y, uint8_t color, uint8_t size);
void LCD_write(char c);
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
