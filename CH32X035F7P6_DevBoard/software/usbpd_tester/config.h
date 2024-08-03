// ===================================================================================
// USB PD Tester User Configurations
// ===================================================================================
//
// Description:
// ------------
// Displays source capabilities of the connected USB PD power supply on an OLED.

#pragma once

// Pin definitions
#define PIN_SCL             PA5       // I2C SCL connected to OLED
#define PIN_SDA             PA6       // I2C SDA connected to OLED
#define PIN_LED             PB1       // pin connected to LED (active low)

// MCU supply voltage
#define USB_VDD             0         // 0: 3.3V, 1: 5V
