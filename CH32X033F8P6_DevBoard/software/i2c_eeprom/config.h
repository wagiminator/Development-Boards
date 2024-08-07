// ===================================================================================
// I2C EEPROM User Configurations
// ===================================================================================
//
// Description:
// ------------
// Transfers content of 24Cxx I2C EEPROM via USB-CDC.

#pragma once

// Pin definitions
#define PIN_LED             PB1       // pin connected to LED
#define PIN_SCL             PA10      // pin connected to serial clock of the I2C bus
#define PIN_SDA             PA11      // pin connected to serial data of the I2C bus

// MCU supply voltage
#define USB_VDD             0         // 0: 3.3V, 1: 5V

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH32X035-USB"
#define SERIAL_STR          "CH32X035CDC"
#define INTERF_STR          "CDC-Serial"
