// ===================================================================================
// USB to Serial Converter User Configurations
// ===================================================================================
//
// Description:
// ------------
// Simple USB-to-Serial Converter.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port.
// - Serial port:  DTR: PA0, RTS: PA1, TXD: PA2, RXD: PA3

#pragma once

// Pin definitions
#define PIN_DTR             PA0       // DTR pin (UART)
#define PIN_RTS             PA1       // RTS pin (UART)
#define PIN_LED             PB1       // pin connected to LED

// MCU supply voltage
#define USB_VDD             1         // 0: 3.3V, 1: 5V

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "USB-to-Serial Converter"
#define SERIAL_STR          "CH32X035CDC"
#define INTERF_STR          "CDC-Serial"
