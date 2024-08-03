// ===================================================================================
// I2C DMA OLED Demo User Configurations
// ===================================================================================
//
// Description:
// ------------
// SSD1306 128x64 Pixels I2C OLED demo with constant screen refresh. 
// This implementation utilizes DMA in circular mode for continuous data transfer of 
// the screen buffer to the OLED. Please note that the I2C bus is occupied the whole 
// time, preventing the use of other I2C devices on the same bus.
// Connect an SSD1306 128x64 Pixels I2C OLED to PC1 (SDA) and PC2 (SCL). 

#pragma once

// Pin defines
#define PIN_LED   PC0       // define LED pin
