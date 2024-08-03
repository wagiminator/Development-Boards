// ===================================================================================
// USB CDC to OLED Adapter User Configurations
// ===================================================================================
//
// Description:
// ------------
// The code implements a simple USB controlled 128x64 pixels I2C OLED display.
// The CH32X03x builds a USB communication device class (CDC) for serial communication
// via USB. Text messages of all kinds can be sent via the USB interface and shown
// on the OLED display.
//
// Operating Instructions:
// -----------------------
// - Connect the I2C OLED to the board (define pins below).
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't 
//   matter).
// - Send a text message, it should be displayed on the OLED.

#pragma once

// Pin definitions
#define PIN_SCL             PA5       // pin connected to serial clock of the I2C bus
#define PIN_SDA             PA6       // pin connected to serial data of the I2C bus
#define PIN_LED             PB1       // pin connected to LED (active low)

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
#define PROD_STR            "USB-to-OLED"
#define SERIAL_STR          "CH32X035CDC"
#define INTERF_STR          "CDC-Serial"
