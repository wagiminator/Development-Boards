// ===================================================================================
// SSD1306 128x64 Pixels OLED Terminal Functions                              * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306 128x64 pixels
// I2C OLED for the display of text in the context of emulating a terminal output.
//
// Functions available:
// --------------------
// OLED_init()              Init OLED display
// OLED_clear()             Clear screen of OLED display
// OLED_write(c)            Write a character or handle control characters
// OLED_print(s)            Print string on OLED display
// OLED_println(s)          Print string with newline
//
// References:
// -----------
// - Neven Boyanov: https://github.com/tinusaur/ssd1306xled
// - Stephen Denne: https://github.com/datacute/Tiny4kOLED
// - David Johnson-Davies: http://www.technoblogy.com/show?TV4
// - TinyOLEDdemo: https://github.com/wagiminator/attiny13-tinyoleddemo
// - TinyTerminal: https://github.com/wagiminator/ATtiny85-TinyTerminal
// - USB2OLED: https://github.com/wagiminator/CH552-USB-OLED
//
// 2022 by Stefan Wagner: https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "i2c.h"

void OLED_init(void);           // OLED init function
void OLED_clear(void);          // OLED clear screen
void OLED_write(char c);        // OLED write a character or handle control characters
void OLED_print(char* str);     // OLED print string
void OLED_println(char* str);   // OLED print string with newline
