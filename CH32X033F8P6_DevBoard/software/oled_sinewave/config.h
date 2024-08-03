// ===================================================================================
// OLED Sinewave Text Scroller User Configurations
// ===================================================================================
//
// Description:
// ------------
// Retro-style sine wave animated text scroller for SSD1306 128x32 pixels I2C OLED.

#pragma once

// Pin definitions
#define PIN_SCL             PA10      // pin connected to serial clock of the I2C bus
#define PIN_SDA             PA11      // pin connected to serial data of the I2C bus

// Scroll delay in ms
#define DELAY               30        // decrease value to increase scroll speed

// Message to scroll on OLED (capital letters only)
#define MESSAGE \
  "                     WAGIMINATOR BRINGS BACK 8-BIT RETRO-FEELING TO MODERN " \
  "32-BIT MICROCONTROLLERS! FOR MORE INFORMATION VISIT GITHUB.COM/WAGIMINATOR." \
  "   BARE-METAL CODING RULEZ!"
