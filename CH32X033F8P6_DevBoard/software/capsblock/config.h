// ===================================================================================
// CapsBlock User Configurations
// ===================================================================================
//
// Description:
// ------------
// Tired of accidentally hitting the CAPS-LOCK key? It doesn't have to be. 
// CAPS bLOCK identifies itself as a USB HID keyboard and constantly monitors the
// state of CAPS LOCK. It immediately deactivates it if it has been activated.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - The blocking function is automatically activated.

#pragma once

// Pin definitions
#define PIN_LED             PB1       // pin connected to LED

// MCU supply voltage
#define USB_VDD             1         // 0: 3.3V, 1: 5V

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DB    // PID (shared HID-keyboards)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CAPS bLOCK"
#define SERIAL_STR          "CH32X035HID"
#define INTERF_STR          "HID-Keyboard"
