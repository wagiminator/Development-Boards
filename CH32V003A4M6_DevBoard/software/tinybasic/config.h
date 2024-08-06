// ===================================================================================
// TOYOSHIKI Tiny BASIC User Configurations
// ===================================================================================
//
// Description:
// ------------
// A simple BASIC interpreter based on TOYOSHIKI Tiny BASIC using a UART terminal.
// TX: PD5, RX: PD6, BAUD: 115200, 8N1
//
// Operating Instructions:
// -----------------------
// - Connect the device via USB-to-serial converter to your PC.
// - Open a serial terminal and select the proper COM port.
// - Write and execute your BASIC program.

#pragma once

// Pin defines
#define PIN_LED   PC0       // define LED pin
