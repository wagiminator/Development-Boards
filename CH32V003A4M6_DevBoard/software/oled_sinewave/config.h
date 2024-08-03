// ===================================================================================
// OLED Sinewave Text Scroller Demo User Configurations
// ===================================================================================
//
// Description:
// ------------
// Retro-style sine wave animated text scroller for SSD1306 128x32 pixels I2C OLED.
// Connect the OLED to PC1 (SDA) and PC2 (SCL).

#pragma once

// Scroll delay in ms
#define DELAY 30

// Message to scroll on OLED (capital letters only)
#define MESSAGE \
  "                     WAGIMINATOR BRINGS BACK 8-BIT RETRO-FEELING TO MODERN " \
  "32-BIT MICROCONTROLLERS! FOR MORE INFORMATION VISIT GITHUB.COM/WAGIMINATOR." \
  "   BARE-METAL CODING RULEZ!"
