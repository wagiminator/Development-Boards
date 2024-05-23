// ===================================================================================
// ST7735/ST7789/ILI9340/ILI9341 Color TFT Graphics Functions                 * v1.1 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "st7735_gfx.h"

// ===================================================================================
// Standard ASCII 5x8 Font (chars 32 - 127)
// ===================================================================================
const uint8_t TFT_FONT[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7F, 0x14, 0x7F, 0x14, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1C, 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, 0x08, 0x08, 0x3E, 0x08, 0x08,
  0x00, 0x80, 0x60, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x60, 0x60, 0x00, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x44, 0x42, 0x7F, 0x40, 0x40,
  0x42, 0x61, 0x51, 0x49, 0x46, 0x22, 0x41, 0x49, 0x49, 0x36, 0x18, 0x14, 0x12, 0x7F, 0x10,
  0x2F, 0x49, 0x49, 0x49, 0x31, 0x3E, 0x49, 0x49, 0x49, 0x32, 0x03, 0x01, 0x71, 0x09, 0x07,
  0x36, 0x49, 0x49, 0x49, 0x36, 0x26, 0x49, 0x49, 0x49, 0x3E, 0x00, 0x36, 0x36, 0x00, 0x00,
  0x00, 0x80, 0x68, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x22, 0x14, 0x08, 0x00, 0x02, 0x01, 0x51, 0x09, 0x06, 0x3E, 0x41, 0x5D, 0x55, 0x5E,
  0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E, 0x41, 0x41, 0x41, 0x22,
  0x7F, 0x41, 0x41, 0x22, 0x1C, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x7F, 0x09, 0x09, 0x09, 0x01,
  0x3E, 0x41, 0x49, 0x49, 0x3A, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x41, 0x41, 0x7F, 0x41, 0x41,
  0x20, 0x40, 0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40, 0x40,
  0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E,
  0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x41, 0xC1, 0xBE, 0x7F, 0x09, 0x19, 0x29, 0x46,
  0x26, 0x49, 0x49, 0x49, 0x32, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x3F, 0x40, 0x40, 0x40, 0x3F,
  0x1F, 0x20, 0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x63, 0x14, 0x08, 0x14, 0x63,
  0x07, 0x08, 0x70, 0x08, 0x07, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x7F, 0x41, 0x41, 0x00,
  0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x08, 0x04, 0x02, 0x04, 0x08,
  0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x06, 0x09, 0x09, 0x06, 0x20, 0x54, 0x54, 0x54, 0x78,
  0x7F, 0x44, 0x44, 0x44, 0x38, 0x38, 0x44, 0x44, 0x44, 0x28, 0x38, 0x44, 0x44, 0x44, 0x7F,
  0x38, 0x54, 0x54, 0x54, 0x18, 0x08, 0xFE, 0x09, 0x01, 0x02, 0x18, 0xA4, 0xA4, 0xA4, 0x78,
  0x7F, 0x04, 0x04, 0x04, 0x78, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, 0x80, 0x84, 0x7D, 0x00,
  0x41, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x41, 0x7F, 0x40, 0x00, 0x7C, 0x04, 0x7C, 0x04, 0x78,
  0x7C, 0x04, 0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44, 0x38, 0xFC, 0x24, 0x24, 0x24, 0x18,
  0x18, 0x24, 0x24, 0x24, 0xFC, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x08, 0x54, 0x54, 0x54, 0x20,
  0x04, 0x3F, 0x44, 0x40, 0x20, 0x3C, 0x40, 0x40, 0x40, 0x3C, 0x1C, 0x20, 0x40, 0x20, 0x1C,
  0x3C, 0x40, 0x30, 0x40, 0x3C, 0x44, 0x28, 0x10, 0x28, 0x44, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C,
  0x44, 0x64, 0x54, 0x4C, 0x44, 0x08, 0x08, 0x36, 0x41, 0x41, 0x00, 0x00, 0xFF, 0x00, 0x00,
  0x41, 0x41, 0x36, 0x08, 0x08, 0x08, 0x04, 0x08, 0x10, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// ===================================================================================
// 13x32 7-Segment Font (0 - 9)
// ===================================================================================
#if TFT_SEG_FONT == 1
const uint8_t TFT_FONT_SEG1[] = {
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 0
  0x7F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 
  0xFF, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, // 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 
  0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 2
  0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0xFF, 0xFE, 0xFC, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x60, 0x40, 0x00, 
  0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 3
  0x00, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x40, 0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, // 4
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, // 5
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x40, 0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, // 6
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 
  0xFF, 0xFE, 0xFC, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 7
  0x7F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFE, 0xFF, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 8
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0xFF, 0xFE, 0xFC, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x1F, 0x4F, 0x67, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F, 
  0xFC, 0xF9, 0xF3, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0xF3, 0xF9, 0xFC, // 9
  0x7F, 0x3F, 0x9F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x9F, 0x3F, 0x7F, 
  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFC, 0xFE, 0xFF, 
  0x00, 0x40, 0x60, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x67, 0x4F, 0x1F
};
#endif

// ===================================================================================
// 5x16 7-Segment Font (0 - 9)
// ===================================================================================
#if TFT_SEG_FONT == 2
const uint8_t TFT_FONT_SEG2[] = {
  0x7C, 0x02, 0x02, 0x02, 0x7C, 0x1F, 0x20, 0x20, 0x20, 0x1F, // 0  0
  0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x1F, // 1  1
  0x00, 0x82, 0x82, 0x82, 0x7C, 0x1F, 0x20, 0x20, 0x20, 0x00, // 2  2
  0x00, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x20, 0x20, 0x20, 0x1F, // 3  3
  0x7C, 0x80, 0x80, 0x80, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x1F, // 4  4
  0x7C, 0x82, 0x82, 0x82, 0x00, 0x00, 0x20, 0x20, 0x20, 0x1F, // 5  5
  0x7C, 0x82, 0x82, 0x82, 0x00, 0x1F, 0x20, 0x20, 0x20, 0x1F, // 6  6
  0x7C, 0x02, 0x02, 0x02, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x1F, // 7  7
  0x7C, 0x82, 0x82, 0x82, 0x7C, 0x1F, 0x20, 0x20, 0x20, 0x1F, // 8  8
  0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00, 0x20, 0x20, 0x20, 0x1F  // 9  9
};
#endif

// ===================================================================================
// SPI Functions
// ===================================================================================
#define TFT_sendData      SPI_write
#define TFT_sendCommand   TFT_SPI_command
#define TFT_sendCommand2  TFT_SPI_command2

// SPI init function (setup pins)
void TFT_SPI_init(void) {
  PIN_high(TFT_PIN_CS);                           // setup control pins
  PIN_high(TFT_PIN_DC);
  PIN_output(TFT_PIN_CS);
  PIN_output(TFT_PIN_DC);
  #if TFT_INIT_SPI > 0
  SPI_init();
  SPI_setCPOL(1);
  #endif
}

// Send a command to the display
void TFT_SPI_command(uint8_t cmd) {
  while(SPI_busy());
  PIN_low(TFT_PIN_DC);
  TFT_sendData(cmd);
  while(SPI_busy());
  PIN_high(TFT_PIN_DC);
}

// Send a command followed by two data bytes
void TFT_SPI_command2(uint8_t c, uint16_t d1, uint16_t d2) {
  TFT_sendCommand(c);
  TFT_sendData(d1>>8); TFT_sendData(d1); TFT_sendData(d2>>8); TFT_sendData(d2);
}

// ===================================================================================
// TFT Control Functions
// ===================================================================================

// Init TFT
void TFT_init(void) {
  TFT_SPI_init();                                 // initialize SPI first
  #if TFT_BOOT_TIME > 0
  DLY_ms(TFT_BOOT_TIME);                          // time for the TFT to boot up
  #endif

  PIN_low(TFT_PIN_CS);
  TFT_sendCommand(0x01);                          // software reset
  DLY_ms(250);                                    // delay 250 ms
  TFT_sendCommand(0x36); TFT_sendData(TFT_ROTATE<<5 | TFT_BGR<<3); // set orientation and rgb/bgr
  #if TFT_COLORBITS == 16
  TFT_sendCommand(0x3A); TFT_sendData(0x55);      // set color mode - 16-bit color
  #else
  TFT_sendCommand(0x3A); TFT_sendData(0x03);      // set color mode - 12-bit color
  #endif
  TFT_sendCommand(0x20 + TFT_INVERT);             // invert
  TFT_sendCommand(0x11);                          // out of sleep mode
  DLY_ms(150);
  TFT_sendCommand(0x29);                          // turn on display
  DLY_ms(150);
  #if TFT_CS_CONTROL > 0
  PIN_high(TFT_PIN_CS);
  #endif
}

// Invert display
void TFT_invert(uint8_t yes) {
  #if TFT_CS_CONTROL > 0
  PIN_low(TFT_PIN_CS);
  #endif
  TFT_sendCommand(0x20 + yes);
  #if TFT_CS_CONTROL > 0
  PIN_high(TFT_PIN_CS);
  #endif
}

// Put display into sleep mode
void TFT_sleep(uint8_t yes) {
  #if TFT_CS_CONTROL > 0
  PIN_low(TFT_PIN_CS);
  #endif
  TFT_sendCommand(0x11 - yes);
  #if TFT_CS_CONTROL > 0
  PIN_high(TFT_PIN_CS);
  #endif
}

// ===================================================================================
// TFT Graphics Functions
// ===================================================================================

// Clear screen
void TFT_clear(void) {
  #if TFT_CS_CONTROL > 0
    PIN_low(TFT_PIN_CS);
  #endif
  TFT_sendCommand2(TFT_CASET, TFT_YOFF, TFT_YOFF + TFT_HEIGHT - 1); // column address set
  TFT_sendCommand2(TFT_RASET, TFT_XOFF, TFT_XOFF + TFT_WIDTH - 1);  // row address set
  #if TFT_COLORBITS == 16
    TFT_sendCommand(0x3A); TFT_sendData(0x03);                      // 12-bit color
  #endif
  TFT_sendCommand(TFT_RAMWR);                                       // write to RAM
  for(uint32_t i=TFT_WIDTH*TFT_HEIGHT*12/8; i; i--) {
    TFT_sendData(0);
  }
  #if TFT_COLORBITS == 16
    TFT_sendCommand(0x3A); TFT_sendData(0x05);                      // back to 16-bit color
  #endif
  #if TFT_CS_CONTROL > 0
    while(SPI_busy());
    PIN_high(TFT_PIN_CS);
  #endif
}

// Set pixel at position (x,y) with color
void TFT_setPixel(int16_t x, int16_t y, uint16_t color) {
  #if TFT_PORTRAIT == 0
    if((x < 0) || (x >= TFT_WIDTH) || (y < 0) || (y >= TFT_HEIGHT)) return;
    #if TFT_XFLIP > 0
      uint16_t row    = (TFT_XOFF + TFT_WIDTH  - 1) - x;
    #else
      uint16_t row    = TFT_XOFF + x;
    #endif
    #if TFT_YFLIP > 0
      uint16_t column = TFT_YOFF + y;
    #else
      uint16_t column = (TFT_YOFF + TFT_HEIGHT - 1) - y;
    #endif

  #else
    if((x < 0) || (x >= TFT_HEIGHT) || (y < 0) || (y >= TFT_WIDTH)) return;
    #if TFT_YFLIP > 0
      uint16_t row    = TFT_XOFF + y;
    #else
      uint16_t row    = (TFT_XOFF + TFT_WIDTH  - 1) - y;
    #endif
    #if TFT_XFLIP > 0
      uint16_t column = TFT_YOFF + x;
    #else
      uint16_t column = (TFT_YOFF + TFT_HEIGHT - 1) - x;
    #endif
  #endif

  #if TFT_CS_CONTROL > 0
    PIN_low(TFT_PIN_CS);
  #endif
  TFT_sendCommand2(TFT_CASET, column, column);    // column address set
  TFT_sendCommand2(TFT_RASET, row, row);          // row address set
  TFT_sendCommand(TFT_RAMWR);                     // write to RAM
  TFT_sendData(color >> 8); TFT_sendData(color);  // write pixel color
  #if TFT_CS_CONTROL > 0
    while(SPI_busy());
    PIN_high(TFT_PIN_CS);
  #endif
}

// Draw filled rectangle starting from (x,y), width (w), height (h), color
void TFT_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if((w < 1) || (h < 1)) return;
  #if TFT_PORTRAIT == 0
    #if TFT_XFLIP > 0
      int16_t row2 = (TFT_XOFF + TFT_WIDTH  - 1) - x;
      int16_t row1 = row2 - w + 1;
    #else
      int16_t row1 = TFT_XOFF + x;
      int16_t row2 = row1 + w - 1;
    #endif
    #if TFT_YFLIP > 0
      int16_t col1 = TFT_YOFF + y;
      int16_t col2 = col1 + h - 1;
    #else
      int16_t col2 = (TFT_YOFF + TFT_HEIGHT - 1) - y;
      int16_t col1 = col2 - h + 1;
    #endif

  #else
    #if TFT_YFLIP > 0
      int16_t row1 = TFT_XOFF + y;
      int16_t row2 = row1 + h - 1;
    #else
      int16_t row2 = (TFT_XOFF + TFT_WIDTH  - 1) - y;
      int16_t row1 = row2 - h + 1;
    #endif
    #if TFT_XFLIP > 0
      int16_t col1 = TFT_YOFF + x;
      int16_t col2 = col1 + w - 1;
    #else
      int16_t col2 = (TFT_YOFF + TFT_HEIGHT - 1) - x;
      int16_t col1 = col2 - w + 1;
    #endif
  #endif
  
  if((col1 >= TFT_YOFF + TFT_HEIGHT) || (col2 < TFT_YOFF) || (row1 >= TFT_XOFF + TFT_WIDTH) || (row2 < TFT_XOFF)) return;
  if(col1 < TFT_YOFF) col1 = TFT_YOFF;
  if(col2 >= TFT_YOFF + TFT_HEIGHT) col2 = TFT_YOFF + TFT_HEIGHT - 1;
  if(row1 < TFT_XOFF) row1 = TFT_XOFF;
  if(row2 >= TFT_XOFF + TFT_WIDTH)  row2 = TFT_XOFF + TFT_WIDTH  - 1;

  #if TFT_CS_CONTROL > 0
    PIN_low(TFT_PIN_CS);
  #endif
  TFT_sendCommand2(TFT_CASET, col1, col2);        // column address set
  TFT_sendCommand2(TFT_RASET, row1, row2);        // row address set
  TFT_sendCommand(TFT_RAMWR);                     // write to RAM

  #if TFT_COLORBITS == 16
    for(uint32_t i=(col2-col1+1)*(row2-row1+1); i; i--) {
      TFT_sendData(color >> 8); TFT_sendData(color);
    }
  #else
    for(uint32_t i=((col2-col1+1)*(row2-row1+1)+1)>>1; i; i--) {
      TFT_sendData(color >> 8); TFT_sendData(color | (color >> 12)); TFT_sendData(color >> 4);
    }
  #endif

  #if TFT_CS_CONTROL > 0
    while(SPI_busy());
    PIN_high(TFT_PIN_CS);
  #endif
}

// Draw vertical line starting from (x,y), height (h), color
void TFT_drawVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  TFT_fillRect(x, y, 1, h, color);
}

// Draw horizontal line starting from (x,y), width (w), color
void TFT_drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  TFT_fillRect(x, y, w, 1, color);
}

// Draw line from position (x0,y0) to (x1,y1) with color
// (Bresenham's line algorithm)
void TFT_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t dx = TFT_abs(x1 - x0);
  int16_t sx = x0 < x1 ? 1 : -1;
  int16_t dy = -TFT_abs(y1 - y0);
  int16_t sy = y0 < y1 ? 1 : -1;
  int16_t error = dx + dy;
    
  while(1) {
    TFT_setPixel(x0, y0, color);
    if(x0 == x1 && y0 == y1) break;
    int16_t e2 = error * 2;
    if(e2 >= dy) {
      if(x0 == x1) break;
      error += dy;
      x0 += sx;
    }
    if(e2 <= dx) {
      if(y0 == y1) break;
      error += dx;
      y0 += sy;
    }
  }
}

// Draw rectangle starting from (x,y), width (w), height (h), color
void TFT_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  TFT_drawHLine(x    , y,     w, color);
  TFT_drawHLine(x    , y+h-1, w, color);
  TFT_drawVLine(x    , y,     h, color);
  TFT_drawVLine(x+w-1, y,     h, color);
}

// Draw circle, center at position (x0,y0), radius (r), color
// (midpoint circle algorithm)
void TFT_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -(r << 1);
  int16_t x = 0;
  int16_t y = r;

  while(x <= y) {
    TFT_setPixel(x0 + x, y0 + y, color);
    TFT_setPixel(x0 - x, y0 + y, color);
    TFT_setPixel(x0 + x, y0 - y, color);
    TFT_setPixel(x0 - x, y0 - y, color);
    TFT_setPixel(x0 + y, y0 + x, color);
    TFT_setPixel(x0 - y, y0 + x, color);
    TFT_setPixel(x0 + y, y0 - x, color);
    TFT_setPixel(x0 - y, y0 - x, color);

    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    ddF_x += 2;
    f += ddF_x;
    x++;
  }
}

// Draw filled circle, center at position (x0,y0), radius (r), color
// (midpoint circle algorithm)
void TFT_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -(r << 1);
  int16_t x = 0;
  int16_t y = r;

  while(x <= y) {
    TFT_drawVLine(x0 - x, y0 - y, (y << 1) + 1, color);
    TFT_drawVLine(x0 + x, y0 - y, (y << 1) + 1, color);
    TFT_drawVLine(x0 - y, y0 - x, (x << 1) + 1, color);
    TFT_drawVLine(x0 + y, y0 - x, (x << 1) + 1, color);

    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  }
}

// ===================================================================================
// TFT Bitmap Functions
// ===================================================================================

// Draw bitmap at position (x0,y0), width (w), hight (h), pointer to bitmap (*bmp)
void TFT_drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint16_t* bmp) {
  for(int16_t y=y0; y<y0+h; y++) {
    for(int16_t x=x0; x<x0+w; x++) {
        TFT_setPixel(x, y, *bmp++);
    }
  }
}

// Draw sprite (monochrome bitmap with transparent background)
void TFT_drawSprite(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp, uint16_t color) {
  for(int16_t y=y0; y<y0+h; y+=8) {
    for(int16_t x=x0; x<x0+w; x++) {
      uint8_t line = *bmp++;
      for(int16_t i=y; i<y+8; i++, line>>=1) {
        if(line & 1) TFT_setPixel(x, i, color);
      }
    }
  }
}

// ===================================================================================
// TFT Character Functions
// ===================================================================================

// Variables
int16_t  TFT_cx, TFT_cy;                          // cursor position
uint16_t TFT_cc;                                  // color
uint8_t  TFT_cs;                                  // size

// Set text cursor position, color and size
void TFT_cursor(int16_t x, int16_t y) {
  TFT_cx = x; TFT_cy = y;  
}

// Set text color
void TFT_color(uint16_t color) {
  TFT_cc = color;
}

// Set text size
void TFT_size(uint8_t size) {
  TFT_cs = size;
}

// Draw character (c) at cursor position and settings
void TFT_drawChar(char c) {
  uint16_t ptr = c - 32;
  ptr += ptr << 2;
  for(uint8_t i=5; i; i--) {
    int16_t y1 = TFT_cy;
    uint8_t line = TFT_FONT[ptr++];
    for(uint8_t j=8; j; j--, line>>=1, y1 += TFT_cs) {
      if(line & 1) {
        if(TFT_cs == 1) TFT_setPixel(TFT_cx, y1, TFT_cc);
        else {
          TFT_fillRect(TFT_cx, y1, TFT_cs, TFT_cs, TFT_cc);
        }
      }
    }
    TFT_cx += TFT_cs;
  }
  TFT_cx += TFT_cs;
}

// Converts bit pattern abcdefgh into aabbccddeeffgghh
uint16_t TFT_stretch(uint16_t x) {
  x = (x & 0xF0)<<4 | (x & 0x0F);
  x = (x<<2 | x) & 0x3333;
  x = (x<<1 | x) & 0x5555;
  return x | x<<1;
}

// Draw character (c) at cursor position and color, double-size, smoothed
// (David Johnson-Davies' Smooth Big Text algorithm)
void TFT_smoothChar(char c) {
  uint16_t ptr = c - 32;
  ptr += ptr << 2;
  uint16_t col0L, col0R, col1L, col1R;
  uint8_t col0 = TFT_FONT[ptr++];
  col0L = TFT_stretch(col0);
  col0R = col0L;
  for(uint8_t col=5; col; col--) {
    uint8_t col1 = TFT_FONT[ptr++];
    if(col == 1) col1 = 0;
    col1L = TFT_stretch(col1);
    col1R = col1L;    
    for(int8_t i=6; i>=0; i--) {
      for(int8_t j=1; j<3; j++) {
        if(((col0>>i & 0b11) == (3 - j)) && ((col1>>i & 0b11) == j)) {
          col0R = col0R | 1<<((i << 1) + j);
          col1L = col1L | 1<<((i << 1) + 3 - j);
        }
      }
    }
    int16_t y1 = TFT_cy;
    for(int8_t i=16; i; i--, col0L>>=1, col0R>>=1, y1++) {
      if(col0L & 1) TFT_setPixel(TFT_cx, y1, TFT_cc);
      if(col0R & 1) TFT_setPixel(TFT_cx+1, y1, TFT_cc);
    }
    col0 = col1; col0L = col1L; col0R = col1R; TFT_cx += 2;
  }
  TFT_cx += 2;
}

// Draw character (c) at cursor position and color, v-stretched
void TFT_stretchChar(char c) {
  uint16_t ptr = c - 32;
  ptr += ptr << 2;
  for(uint8_t col=5; col; col--) {
    uint8_t col0 = TFT_FONT[ptr++];
    int16_t y1 = TFT_cy;
    for(uint8_t i=8; i; i--, col0>>=1, y1+=2) {
      if(col0 & 1) {
        TFT_setPixel(TFT_cx, y1, TFT_cc);
        TFT_setPixel(TFT_cx, y1+1, TFT_cc);
      }
    }
    TFT_cx++;
  }
  TFT_cx++;
}

// ===================================================================================
// TFT Text Functions
// ===================================================================================

// Print string (str) at cursor position
void TFT_print(char* str) {
  while(*str) {
    if(TFT_cs <= 8) {
      TFT_drawChar(*str++);
      continue;
    }
    if(TFT_cs == TFT_SMOOTH) {
      TFT_smoothChar(*str++);
      continue;
    }
    TFT_stretchChar(*str++);
  }
}

// Print value as 7-segment digits (BCD conversion by substraction method)
#if TFT_SEG_FONT > 0
void TFT_printSegment(uint16_t value, uint8_t digits, uint8_t lead, uint8_t decimal) {
  static const uint16_t DIVIDER[] = {1, 10, 100, 1000, 10000};
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint16_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(digits == decimal) leadflag++;             // end leading characters before decimal
    if(leadflag || !lead) {
      uint16_t ptr = (uint16_t)digitval;          // character pointer
      #if TFT_SEG_FONT == 1
      ptr = (ptr << 5) + (ptr << 4) + (ptr << 2); // -> ptr = c * 13 * 4;
      TFT_drawSprite(TFT_cx, TFT_cy, 13, 32, (uint8_t*)&TFT_FONT_SEG1[ptr], TFT_cc);
      #elif TFT_SEG_FONT == 2
      ptr = (ptr << 3) + (ptr << 1);              // -> ptr = c * 5 * 2;
      TFT_drawSprite(TFT_cx, TFT_cy, 5, 16, (uint8_t*)&TFT_FONT_SEG2[ptr], TFT_cc);
      #endif
    }
    #if TFT_SEG_FONT == 1
    TFT_cx += TFT_SEG_SPACE + 13;
    #elif TFT_SEG_FONT == 2
    TFT_cx += TFT_SEG_SPACE + 5;
    #endif
    if(decimal && (digits == decimal)) {
      #if TFT_SEG_FONT == 1
      TFT_fillRect(TFT_cx, TFT_cy + 28, 3, 3, TFT_cc);  // print decimal point
      TFT_cx += TFT_SEG_SPACE + 3;
      #elif TFT_SEG_FONT == 2
      TFT_fillRect(TFT_cx, TFT_cy + 12, 2, 2, TFT_cc);  // print decimal point
      TFT_cx += TFT_SEG_SPACE + 2;
      #endif
    }
  }
}
#endif  // TFT_SEG_FONT > 0

// Write a character (for printf)
#if TFT_PRINT == 1
void TFT_write(char c) {
  c &= 0x7f;
  if(c >= 32) {
    if(TFT_cs <= 8) {
      TFT_drawChar(c);
      return;
    }
    if(TFT_cs == TFT_SMOOTH) {
      TFT_smoothChar(c);
      return;
    }
    TFT_stretchChar(c);
    return;
  }
  if(c == '\n') {
    TFT_cx = 0;
    if(TFT_cs <= 8) TFT_cy += TFT_cs << 3;
    else TFT_cy += 16;
  }
}
#endif  // TFT_PRINT == 1
