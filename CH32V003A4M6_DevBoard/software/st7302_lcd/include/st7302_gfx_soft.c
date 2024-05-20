// ===================================================================================
// ST7302 250x122 Pixels Monochrome Low-Power LCD Graphics Functions          * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "st7302_gfx_soft.h"

// ===================================================================================
// Standard ASCII 5x8 Font (chars 32 - 127)
// ===================================================================================
const uint8_t LCD_FONT[] = {
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
#if LCD_SEG_FONT == 1
const uint8_t LCD_FONT_SEG1[] = {
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
#if LCD_SEG_FONT == 2
const uint8_t LCD_FONT_SEG2[] = {
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
#define LCD_sendData      LCD_SPI_write
#define LCD_readData      LCD_SPI_read
#define LCD_sendCommand   LCD_SPI_command
#define LCD_sendCommand2  LCD_SPI_command2

// SPI init function (setup pins)
void LCD_SPI_init(void) {
  PIN_high(LCD_PIN_CS);                           // setup control pins
  PIN_high(LCD_PIN_DC);
  PIN_high(LCD_PIN_SCL);
  PIN_output(LCD_PIN_CS);
  PIN_output(LCD_PIN_DC);
  PIN_output(LCD_PIN_SCL);
  PIN_output(LCD_PIN_SDA);
}

// Transmit one data byte
void LCD_SPI_write(uint8_t data) {
  for(uint8_t i=8; i; i--, data <<= 1) {          // shift out 8 bits, MSB first
    PIN_low(LCD_PIN_SCL);                         // SCL low -> prepare next bit
    if(data & 0x80) PIN_high(LCD_PIN_SDA);        // set SDA according to data bit
    else            PIN_low(LCD_PIN_SDA);
    PIN_high(LCD_PIN_SCL);                        // SCL high -> shift bit out
  }
}

// Send a command to the display
void LCD_SPI_command(uint8_t cmd) {
  PIN_low(LCD_PIN_DC);
  LCD_SPI_write(cmd);
  PIN_high(LCD_PIN_DC);
}

// Send a command followed by two data bytes
void LCD_SPI_command2(uint8_t c, uint8_t d1, uint8_t d2) {
  LCD_SPI_command(c); LCD_sendData(d1); LCD_sendData(d2);
}

// ===================================================================================
// LCD Control Functions
// ===================================================================================

// Init LCD
void LCD_init(void) {
  LCD_SPI_init();                                 // initialize SPI first
  #if LCD_BOOT_TIME > 0
    DLY_ms(LCD_BOOT_TIME);                        // time for the LCD to boot up
  #endif

  PIN_low(LCD_PIN_CS);
  LCD_sendCommand(0x01);                          // Software reset
  DLY_ms(250);                                    // delay 250 ms
  LCD_sendCommand(0xEB); LCD_sendData(0x02);      // Enable NVM
  LCD_sendCommand(0xD7); LCD_sendData(0x68);      // NVM Load Control
  LCD_sendCommand(0xD1); LCD_sendData(0x01);      // Booster enable
  LCD_sendCommand(0xC0); LCD_sendData(0x80);      // Gate Voltage Setting VGH=12V; VGL=-5V
  LCD_sendCommand(0xC1);                          // Source Voltage Control 1
  LCD_sendData(0x28); LCD_sendData(0x28);
  LCD_sendData(0x28); LCD_sendData(0x28);
  LCD_sendData(0x14); LCD_sendData(0x00);
  LCD_sendCommand(0xC2);                          // Source Voltage Control 2
  LCD_sendCommand(0xCB); LCD_sendData(0x14);      // VCOMH Voltage Setting 4V
  LCD_sendCommand(0xB4);                          // Update Period Gate EQ Control
  LCD_sendData(0xE5); LCD_sendData(0x77);
  LCD_sendData(0xF1); LCD_sendData(0xFF);
  LCD_sendData(0xFF); LCD_sendData(0x4F);
  LCD_sendData(0xF1); LCD_sendData(0xFF);
  LCD_sendData(0xFF); LCD_sendData(0x4F);
  LCD_sendCommand(0xB0); LCD_sendData(0x64);      // Duty Setting
  LCD_sendCommand(0x11);                          // Out of sleep mode
  DLY_ms(100);
  LCD_sendCommand(0xC7); LCD_sendData(0xA6); LCD_sendData(0xE9);  // OSC Enable
  LCD_sendCommand(0x36); LCD_sendData(0x20);      // Memory Data Access Control
  LCD_sendCommand(0x3A); LCD_sendData(0x11);      // Data Format Select
  LCD_sendCommand(0xB8); LCD_sendData(0x09);      // Panel Setting
  LCD_sendCommand(0xD0); LCD_sendData(0x1F);      // Unknown command??
  LCD_sendCommand(0x21);                          // Display Inversion On
  LCD_sendCommand(0x29);                          // Display on
  LCD_sendCommand(0xB9); LCD_sendData(0xE3);      // Clear RAM
  DLY_ms(100);
  LCD_sendCommand(0xB9); LCD_sendData(0x23);      // Source Setting Off
  LCD_sendCommand(0x72); LCD_sendData(0x00);      // Destress Off??
  LCD_sendCommand(0x39);                          // Low Power Mode ON
  DLY_ms(100);
  PIN_high(LCD_PIN_CS);
}

// Invert display
void LCD_invert(uint8_t yes) {
  PIN_low(LCD_PIN_CS);
  LCD_sendCommand(0x20 + yes);
  PIN_high(LCD_PIN_CS);
}

// Put display into sleep mode
void LCD_sleep(uint8_t yes) {
  PIN_low(LCD_PIN_CS);
  LCD_sendCommand(0x11 - yes);
  PIN_high(LCD_PIN_CS);
}

// Plot a 12x2 block at row, col
void LCD_writeBlock(uint8_t row, uint8_t column, uint32_t block) {
  PIN_low(LCD_PIN_CS);
  LCD_sendCommand2(LCD_CASET, LCD_YOFF + column, LCD_YOFF + column);
  LCD_sendCommand2(LCD_RASET, LCD_XOFF + row, LCD_XOFF + row);
  LCD_sendCommand(LCD_RAMWR);
  LCD_sendData(block >> 16); LCD_sendData(block >> 8); LCD_sendData(block);
  PIN_high(LCD_PIN_CS);
}

// Read a 12x2 block at row, col
uint32_t LCD_readBlock(uint8_t row, uint8_t column) {
  PIN_low(LCD_PIN_CS);
  LCD_sendCommand2(LCD_CASET, LCD_YOFF + column, LCD_YOFF + column);
  LCD_sendCommand2(LCD_RASET, LCD_XOFF + row, LCD_XOFF + row);
  LCD_sendCommand(LCD_RAMRD);
  uint32_t block = 0;
  PIN_input_PU(LCD_PIN_SDA);                      // data in
  for(uint8_t i=25; i; i--) {                     // shift in dummy bit + 24 bits, MSB first
    PIN_low(LCD_PIN_SCL);                         // SCL low -> shift bit in
    block <<= 1;                                  // shift data byte
    PIN_high(LCD_PIN_SCL);                        // SCL high -> prepare next bit
    if(PIN_read(LCD_PIN_SDA)) block |= 1;         // set data bit according to SDA
  }
  PIN_output(LCD_PIN_SDA);                        // back to data out
  PIN_high(LCD_PIN_CS);
  return block;
}

// ===================================================================================
// LCD Graphics Functions
// ===================================================================================

// Global Variables
uint8_t row0 = 255 >> 1, column0 = 0;
uint32_t block;

// Clear screen
void LCD_clear(void) {
  PIN_low(LCD_PIN_CS);
  LCD_sendCommand2(LCD_CASET, LCD_YOFF, LCD_YOFF + ((LCD_HEIGHT - 1) / 12));  // Column Address Set
  LCD_sendCommand2(LCD_RASET, LCD_XOFF, LCD_XOFF + ((LCD_WIDTH - 1) >> 1));   // Row Address Set
  LCD_sendCommand(LCD_RAMWR);                     // Write to RAM
  PIN_low(LCD_PIN_SDA);
  for(uint16_t i=LCD_WIDTH*((LCD_HEIGHT + 11) / 12 * 12); i; i--) {
    PIN_low(LCD_PIN_SCL);
    PIN_high(LCD_PIN_SCL);
  }
  PIN_high(LCD_PIN_CS);
  row0 = 255 >> 1; column0 = 0;
}

// Set pixel at position (x,y) with color (0: clear pixel, 1: set pixel, 2: invert pixel)
void LCD_setPixel(int16_t x, int16_t y, uint8_t color) {
  #if LCD_PORTRAIT == 0
    if((x < 0) || (x >= LCD_WIDTH) || (y < 0) || (y >= LCD_HEIGHT)) return;
    #if LCD_FLIP > 0
      x = (int16_t)(LCD_WIDTH  - 1) - x;
      y = (int16_t)(LCD_HEIGHT - 1) - y;
    #endif
    uint8_t row = x >> 1, column = y / 12;
    uint8_t bit = ((~x) & 1) | (11 - y % 12) << 1;
  #else
    if((x < 0) || (x >= LCD_HEIGHT) || (y < 0) || (y >= LCD_WIDTH)) return;
    #if LCD_FLIP > 0
      y = (int16_t)(LCD_WIDTH - 1) - y;
    #else
      x = (int16_t)(LCD_HEIGHT - 1) - x;
    #endif
    uint8_t row = y >> 1, column = x / 12;
    uint8_t bit = ((~y) & 1) | (11 - x % 12) << 1;
  #endif

  if(row != row0 || column != column0) {
    LCD_writeBlock(row0, column0, block);
    block = LCD_readBlock(row, column);
    row0 = row; column0 = column;
  }

  switch(color) {
    case 0: block &= ~((uint32_t)1 << bit); break;
    case 1: block |=  ((uint32_t)1 << bit); break;
    case 2: block ^=  ((uint32_t)1 << bit); break;
  }
}

// Flush screen buffer
void LCD_flush(void) {
  LCD_writeBlock(row0, column0, block);
  row0 = 255 >> 1; column0 = 0;
}

// Get pixel color at (x,y) (0: pixel cleared, 1: pixel set)
uint8_t LCD_getPixel(int16_t x, int16_t y) {
  #if LCD_PORTRAIT == 0
    if((x < 0) || (x >= LCD_WIDTH) || (y < 0) || (y >= LCD_HEIGHT)) return 0;
    #if LCD_FLIP > 0
      x = (int16_t)(LCD_WIDTH  - 1) - x;
      y = (int16_t)(LCD_HEIGHT - 1) - y;
    #endif
    uint8_t  row = x >> 1, column = y / 12;
    uint32_t bit = ((~x) & 1) | (11 - y % 12) << 1;
  #else
    if((x < 0) || (x >= LCD_HEIGHT) || (y < 0) || (y >= LCD_WIDTH)) return 0;
    #if LCD_FLIP > 0
      y = (int16_t)(LCD_WIDTH - 1) - y;
    #else
      x = (int16_t)(LCD_HEIGHT - 1) - x;
    #endif
    uint8_t row = y >> 1, column = x / 12;
    uint8_t bit = ((~y) & 1) | (11 - x % 12) << 1;
  #endif
  return((LCD_readBlock(row, column) >> bit) & 1);
}

// Draw vertical line starting from (x,y), height (h), color (0: cleared, 1: set)
void LCD_drawVLine(int16_t x, int16_t y, int16_t h, uint8_t color) {
  for(int16_t i=y; i<y+h; i++) LCD_setPixel(x, i, color);
}

// Draw horizontal line starting from (x,y), width (w), color (0: cleared, 1: set)
void LCD_drawHLine(int16_t x, int16_t y, int16_t w, uint8_t color) {
  for(int16_t i=x; i<x+w; i++) LCD_setPixel(i, y, color);
}

// Draw line from position (x0,y0) to (x1,y1) with color (0: cleared, 1: set)
// (Bresenham's line algorithm)
void LCD_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
  int16_t dx = LCD_abs(x1 - x0);
  int16_t sx = x0 < x1 ? 1 : -1;
  int16_t dy = -LCD_abs(y1 - y0);
  int16_t sy = y0 < y1 ? 1 : -1;
  int16_t error = dx + dy;
    
  while(1) {
    LCD_setPixel(x0, y0, color);
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

// Draw rectangle starting from (x,y), width (w), height (h), color (0: cleared, 1: set)
void LCD_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
  LCD_drawHLine(x    , y,     w, color);
  LCD_drawHLine(x    , y+h-1, w, color);
  LCD_drawVLine(x    , y,     h, color);
  LCD_drawVLine(x+w-1, y,     h, color);
}

// Draw filled rectangle starting from (x,y), width (w), height (h), color
void LCD_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
  for(int16_t i=x; i<x+w; i++) LCD_drawVLine(i, y, h, color);
}

// Draw circle, center at position (x0,y0), radius (r), color (0: cleared, 1: set)
// (midpoint circle algorithm)
void LCD_drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -(r << 1);
  int16_t x = 0;
  int16_t y = r;

  while(x <= y) {
    LCD_setPixel(x0 + x, y0 + y, color);
    LCD_setPixel(x0 - x, y0 + y, color);
    LCD_setPixel(x0 + x, y0 - y, color);
    LCD_setPixel(x0 - x, y0 - y, color);
    LCD_setPixel(x0 + y, y0 + x, color);
    LCD_setPixel(x0 - y, y0 + x, color);
    LCD_setPixel(x0 + y, y0 - x, color);
    LCD_setPixel(x0 - y, y0 - x, color);

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
void LCD_fillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -(r << 1);
  int16_t x = 0;
  int16_t y = r;

  while(x <= y) {
    LCD_drawVLine(x0 - x, y0 - y, (y << 1) + 1, color);
    LCD_drawVLine(x0 + x, y0 - y, (y << 1) + 1, color);
    LCD_drawVLine(x0 - y, y0 - x, (x << 1) + 1, color);
    LCD_drawVLine(x0 + y, y0 - x, (x << 1) + 1, color);

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
// LCD Bitmap Functions
// ===================================================================================

// Draw bitmap at position (x0,y0), width (w), hight (h), pointer to bitmap (*bmp)
void LCD_drawBitmap(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp) {
  for(int16_t y=y0; y<y0+h; y+=8) {
    for(int16_t x=x0; x<x0+w; x++) {
      uint8_t line = *bmp++;
      for(int16_t i=y; i<y+8; i++, line>>=1) {
        LCD_setPixel(x, i, line & 1);
      }
    }
  }
}

// Draw sprite (bitmap with transparent background)
void LCD_drawSprite(int16_t x0, int16_t y0, int16_t w, int16_t h, const uint8_t* bmp) {
  for(int16_t y=y0; y<y0+h; y+=8) {
    for(int16_t x=x0; x<x0+w; x++) {
      uint8_t line = *bmp++;
      for(int16_t i=y; i<y+8; i++, line>>=1) {
        if(line & 1) LCD_setPixel(x, i, 1);
      }
    }
  }
}

// ===================================================================================
// LCD Text Functions
// ===================================================================================

// Draw character (c) at position (x,y), color, size
void LCD_drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t size) {
  uint16_t ptr = c - 32;
  ptr += ptr << 2;
  for(uint8_t i=6; i; i--) {
    uint8_t line, col;
    int16_t y1 = y;
    line = LCD_FONT[ptr++];
    if(i == 1) line = 0;
    for(uint8_t j=0; j<8; j++, line>>=1) {
      if(line & 1) col = color;
      else col = !color;
      if(size == 1) LCD_setPixel(x, y1++, col);
      else {
        LCD_fillRect(x, y1, size, size, col);
        y1 += size;
      }
    }
    x += size;
  }
}

// Converts bit pattern abcdefgh into aabbccddeeffgghh
uint16_t LCD_stretch(uint16_t x) {
  x = (x & 0xF0)<<4 | (x & 0x0F);
  x = (x<<2 | x) & 0x3333;
  x = (x<<1 | x) & 0x5555;
  return x | x<<1;
}

// Draw character (c) at position (x,y), color, double-size, smoothed
// (David Johnson-Davies' Smooth Big Text algorithm)
void LCD_smoothChar(int16_t x, int16_t y, char c, uint8_t color) {
  uint16_t ptr = c - 32;
  ptr += ptr << 2;
  uint16_t col0L, col0R, col1L, col1R;
  uint8_t col0 = LCD_FONT[ptr++];
  col0L = LCD_stretch(col0);
  col0R = col0L;
  for(uint8_t col=5; col; col--) {
    uint8_t col1 = LCD_FONT[ptr++];
    if(col == 1) col1 = 0;
    col1L = LCD_stretch(col1);
    col1R = col1L;    
    for(int8_t i=6; i>=0; i--) {
      for(int8_t j=1; j<3; j++) {
        if(((col0>>i & 0b11) == (3 - j)) && ((col1>>i & 0b11) == j)) {
          col0R = col0R | 1<<((i << 1) + j);
          col1L = col1L | 1<<((i << 1) + 3 - j);
        }
      }
    }
    int16_t y1 = y;
    if(!color) {
      col0L = ~col0L;
      col0R = ~col0R;
    }
    for(int8_t i=16; i; i--, col0L>>=1, col0R>>=1) {
      LCD_setPixel(x,   y1,   col0L & 1);
      LCD_setPixel(x+1, y1++, col0R & 1);
    }
    col0 = col1; col0L = col1L; col0R = col1R; x += 2;
  }
  LCD_fillRect(x, y, 2, 16, !color);
}

// Draw character (c) at position (x,y), color, v-stretched
void LCD_stretchChar(int16_t x, int16_t y, char c, uint8_t color) {
  uint16_t ptr = c - 32;
  ptr += ptr << 2;
  for(uint8_t col=6; col; col--) {
    uint8_t col0 = LCD_FONT[ptr++];
    if(col == 1) col0 = 0;
    if(!color) col0 = ~col0;
    int16_t y1 = y;
    for(uint8_t i=8; i; i--, col0>>=1) {
      LCD_setPixel(x, y1++, col0 & 1);
      LCD_setPixel(x, y1++, col0 & 1);
    }
    x++;
  }
}

// Print string (str) at position (x,y), color, size
void LCD_print(int16_t x, int16_t y, char* str, uint8_t color, uint8_t size) {
  while(*str) {
    if(size <= 8) {
      LCD_drawChar(x, y, *str++, color, size);
      x += ((size << 2) + (size << 1));
      continue;
    }
    if(size == LCD_SMOOTH) {
      LCD_smoothChar(x, y, *str++, color);
      x += 12;
      continue;
    }
    LCD_stretchChar(x, y, *str++, color);
    x += 6;
  }
}

// ===================================================================================
// LCD 7-Segment Functions
// ===================================================================================

// Print value as 7-segment digits (BCD conversion by substraction method)
#if LCD_SEG_FONT > 0
void LCD_printSegment(int16_t x, int16_t y, uint16_t value, uint8_t digits, uint8_t lead, uint8_t decimal) {
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
      #if LCD_SEG_FONT == 1
      ptr = (ptr << 5) + (ptr << 4) + (ptr << 2); // -> ptr = c * 13 * 4;
      LCD_drawBitmap(x, y, 13, 32, (uint8_t*)&LCD_FONT_SEG1[ptr]);
      #elif LCD_SEG_FONT == 2
      ptr = (ptr << 3) + (ptr << 1);              // -> ptr = c * 5 * 2;
      LCD_drawBitmap(x, y, 5, 16, (uint8_t*)&LCD_FONT_SEG2[ptr]);
      #endif
    }
    #if LCD_SEG_FONT == 1
    x += LCD_SEG_SPACE + 13;
    #elif LCD_SEG_FONT == 2
    x += LCD_SEG_SPACE + 5;
    #endif
    if(decimal && (digits == decimal)) {
      #if LCD_SEG_FONT == 1
      LCD_fillRect(x, y + 28, 3, 3, 1);          // print decimal point
      x += LCD_SEG_SPACE + 3;
      #elif LCD_SEG_FONT == 2
      LCD_fillRect(x, y + 12, 2, 2, 1);          // print decimal point
      x += LCD_SEG_SPACE + 2;
      #endif
    }
  }
}
#endif  // LCD_SEG_FONT > 0

// ===================================================================================
// printf Extension
// ===================================================================================

#if LCD_PRINT == 1

// Variables
int16_t LCD_cx, LCD_cy;                           // cursor position
uint8_t LCD_cc, LCD_cs;                           // color and size

// Set cursor position, color and size
void LCD_cursor(int16_t x, int16_t y, uint8_t color, uint8_t size) {
  LCD_cx = x; LCD_cy = y; LCD_cc = color; LCD_cs = size;
}

// Write a character (for printf)
void LCD_write(char c) {
  c &= 0x7f;
  if(c >= 32) {
    if(LCD_cs <= 8) {
      LCD_drawChar(LCD_cx, LCD_cy, c, LCD_cc, LCD_cs);
      LCD_cx += ((LCD_cs << 2) + (LCD_cs << 1));
      return;
    }
    if(LCD_cs == LCD_SMOOTH) {
      LCD_smoothChar(LCD_cx, LCD_cy, c, LCD_cc);
      LCD_cx += 12;
      return;
    }
    LCD_stretchChar(LCD_cx, LCD_cy, c, LCD_cc);
    LCD_cx += 6;
    return;
  }
  if(c == '\n') {
    LCD_cx = 0;
    if(LCD_cs <= 8) LCD_cy += LCD_cs << 3;
    else LCD_cy += 16;
  }
}

#endif
