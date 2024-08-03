// ===================================================================================
// USB PD Demo User Configurations
// ===================================================================================
//
// Description:
// ------------
// Request the specified target voltage from a USB PD power supply. LED lights up when
// successful.

#pragma once

// Target voltage
#define TARGET_VOLTAGE    9000      // define target voltage in millivolts

// Pin definitions
#define PIN_LED           PB1       // pin connected to LED (active low)

// MCU supply voltage
#define USB_VDD           0         // 0: 3.3V, 1: 5V
