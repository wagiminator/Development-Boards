// ===================================================================================
// Project:   Example for CH32V003
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Retro-style sine wave animated text scroller for SSD1306 128x32 pixels I2C OLED.
// Connect the OLED to PC1 (SDA) and PC2 (SCL).
//
// References:
// -----------
// - CNLohr ch32v003fun:    https://github.com/cnlohr/ch32v003fun
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - TinyOLEDdemo:          https://github.com/wagiminator/attiny13-tinyoleddemo
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
// - Sine Table Creator:    https://www.daycounter.com/Calculators/Sine-Generator-Calculator.phtml
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                           // user configurations
#include <system.h>                           // system functions
#include <i2c_tx.h>                           // I2C TX functions

// Message to scroll on OLED
const char Message[] = MESSAGE;

// Global variables
uint8_t sine_ptr;                             // sine wave table pointer
uint8_t msg_ptr;                              // message pointer
uint8_t shift;                                // character shif

// ===================================================================================
// Sine Wave Look Up Table (quarter wave, amplitude 23, 32 points as nibbles in 16 bytes)
// ===================================================================================
const uint8_t SINE24[]  = {
  0xCB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
  0x33, 0x32, 0x22, 0x11, 0x11, 0x00, 0x00, 0x00
};

// ===================================================================================
// OLED 5x8 Pixels Font (capital letters only)
// ===================================================================================
const uint8_t OLED_FONT[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7F, 0x14, 0x7F, 0x14, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x55, 0x22, 0x50, 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1C, 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, 0x08, 0x08, 0x3E, 0x08, 0x08,
  0x00, 0x00, 0xA0, 0x60, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x60, 0x60, 0x00, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x42, 0x7F, 0x40, 0x00,
  0x42, 0x61, 0x51, 0x49, 0x46, 0x21, 0x41, 0x45, 0x4B, 0x31, 0x18, 0x14, 0x12, 0x7F, 0x10,
  0x27, 0x45, 0x45, 0x45, 0x39, 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x01, 0x71, 0x09, 0x05, 0x03,
  0x36, 0x49, 0x49, 0x49, 0x36, 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x36, 0x36, 0x00, 0x00,
  0x00, 0x56, 0x36, 0x00, 0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x41, 0x22, 0x14, 0x08, 0x02, 0x01, 0x51, 0x09, 0x06, 0x32, 0x49, 0x59, 0x51, 0x3E,
  0x7C, 0x12, 0x11, 0x12, 0x7C, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x3E, 0x41, 0x41, 0x41, 0x22,
  0x7F, 0x41, 0x41, 0x22, 0x1C, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x7F, 0x09, 0x09, 0x09, 0x01,
  0x3E, 0x41, 0x49, 0x49, 0x7A, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x41, 0x7F, 0x41, 0x00,
  0x20, 0x40, 0x41, 0x3F, 0x01, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x7F, 0x40, 0x40, 0x40, 0x40,
  0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x3E, 0x41, 0x41, 0x41, 0x3E,
  0x7F, 0x09, 0x09, 0x09, 0x06, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x7F, 0x09, 0x19, 0x29, 0x46,
  0x46, 0x49, 0x49, 0x49, 0x31, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x3F, 0x40, 0x40, 0x40, 0x3F,
  0x1F, 0x20, 0x40, 0x20, 0x1F, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x63, 0x14, 0x08, 0x14, 0x63,
  0x07, 0x08, 0x70, 0x08, 0x07, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x7F, 0x41, 0x41, 0x00,
  0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x04, 0x02, 0x01, 0x02, 0x04,
  0x40, 0x40, 0x40, 0x40, 0x40
};

// ===================================================================================
// OLED Definitions and Functions
// ===================================================================================

// Global variables
uint8_t OLED_xpos;                            // x position on OLED

// OLED definitions
#define OLED_ADDR       0x78                  // OLED write address
#define OLED_CMD_MODE   0x00                  // set command mode
#define OLED_DAT_MODE   0x40                  // set data mode

// OLED init settings
const uint8_t OLED_INIT_CMD[] = {
  0xA8, 0x1F,         // set multiplex (HEIGHT-1): 0x1F for 128x32, 0x3F for 128x64 
  0x20, 0x01,         // set vertical memory addressing mode
  0x22, 0x00, 0x03,   // set min and max page
  0xDA, 0x02,         // set COM pins hardware configuration to sequential
  0x8D, 0x14,         // enable charge pump
  0xAF,               // switch on OLED
  0xA1, 0xC8          // flip the screen
};

// OLED init function
void OLED_init(void) {
  I2C_init();                                 // init I2C
  DLY_ms(50);                                 // time for the OLED to boot up
  I2C_start(OLED_ADDR);                       // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                   // set command mode and send command bytes ...
  for(uint8_t i = 0; i < sizeof(OLED_INIT_CMD); i++) I2C_write(OLED_INIT_CMD[i]);
  I2C_stop();                                 // stop transmission
}

// OLED set the cursor
void OLED_cursor(uint8_t xpos, uint8_t ypos) {
  OLED_xpos = xpos;                           // set x-position variable
  I2C_start(OLED_ADDR);                       // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                   // set command mode
  I2C_write(xpos & 0x0F);                     // set low nibble of start column
  I2C_write(0x10 | (xpos >> 4));              // set high nibble of start column
  I2C_write(0xB0 | (ypos & 0x07));            // set start page
  I2C_stop();                                 // stop transmission
}

// OLED plot a character
void OLED_plotChar(char c) {
  uint16_t offset = c - 32;                   // calculate position of character in font array
  offset += offset << 2;                      // -> offset = (c - 32) * 5

  for(uint8_t i=0; i<6; i++) {                // character consists of 5 lines + 1 space line
    if(OLED_xpos > 127) return;               // stop if end of OLED
    if(i) {                                   // character line?
      if(!OLED_xpos && (shift > i)) {
        offset++;
        continue;
      }
      uint32_t ch = OLED_FONT[offset++];      // read line of character
      uint8_t  pt = sine_ptr & 0x1F;          // get quarter part of pointer
      if(sine_ptr & 0x20) pt = 0x1F - pt;     // mirror on the y-axis, if necessary
      uint8_t sh = SINE24[pt>>1];             // read sine value
      (pt & 1) ? (sh &= 0x0F) : (sh >>= 4);   // get correct nibble
      if(sine_ptr & 0x40) sh = 0x17 - sh;     // mirror on the x-axis, if necessary
      ch <<= sh;                              // shift char according to sine table value
      for(uint8_t i=4; i; i--) {              // write the shifted line on the OLED ...
        I2C_write(ch);
        ch >>= 8;
      }
    } else {                                  // spacing?
      if(!OLED_xpos && shift) continue;       // not on OLED? -> next line
      for(uint8_t i=4; i; i--) I2C_write(0x00);  // print spacing between characters
    }
    OLED_xpos++; sine_ptr++;                  // increase x position and sine table pointer
  }
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  OLED_init();                                // initialize the OLED

  // Loop
  while(1) {                                  // loop until forever                         
    OLED_cursor(0, 0);                        // set cursor position
    if(++shift > 5) {                         // shift within characters
      shift = 0;                              // reset shift value
      if(++msg_ptr > sizeof(Message) - 2) msg_ptr = 0; // shift one character further
    }
    uint8_t p = msg_ptr;                      // set start character in message
    I2C_start(OLED_ADDR);                     // start transmission to OLED
    I2C_write(OLED_DAT_MODE);                 // set data mode
    for(uint8_t i=22; i; i--) {               // print 22 characters
      OLED_plotChar(Message[p]);              // read and print one character
      if(++p > sizeof(Message) - 2) p = 0;    // increase and limit pointer
    }
    I2C_stop();                               // stop transmission
    sine_ptr -= 2;                            // shift sine wave to the right
    DLY_ms(DELAY);                            // slow down a little
  }
}
