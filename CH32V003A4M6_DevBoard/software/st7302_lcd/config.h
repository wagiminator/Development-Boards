// ===================================================================================
// ST7302 LCD Demo User Configurations
// ===================================================================================
//
// Description:
// ------------
// ST7302 250x122 Pixels Monochrome Low-Power Liquid Crystal Display Graphics Demo. 
// Connect the LCD as follows:
// MCU - Display
// GND - GND
// VCC - VCC (3V3)
// SCL - PC5 (configure below)
// SDA - PC6 (configure below)
// RES - VCC (3V3)
//  DC - PC3 (configure below)
//  CS - PC4 (configure below)
//
// !!! Check operating voltages of your MCU and your display !!!

#pragma once

// Pin defines
#define LCD_PIN_SCL       PC5       // pin connected to SCL (clock) of LCD
#define LCD_PIN_SDA       PC6       // pin connected to SDA (data) of LCD
#define LCD_PIN_DC        PC3       // pin connected to DC (data/command) of LCD
#define LCD_PIN_CS        PC4       // pin connected to CS (select) of LCD
