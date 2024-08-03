// ===================================================================================
// Mouse Wiggler User Configurations
// ===================================================================================
//
// Description:
// ------------
// Mouse Wiggler is a tool that simulates movements of the mouse pointer so that 
// no screen saver is activated and the PC or notebook is not put to sleep. The
// mouse pointer movements are imperceptibly small, so that you can continue to
// work normally on your PC even with the function activated.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID mouse.
// - The wiggle function is automatically activated.

#pragma once

// USB pin defines
#define USB_PORT A     // [A,C,D] GPIO Port to use with D+, D- and DPU
#define USB_PIN_DP 1   // [0-4] GPIO Number for USB D+ Pin
#define USB_PIN_DM 2   // [0-4] GPIO Number for USB D- Pin
//#define USB_PIN_DPU 5  // [0-7] GPIO for feeding the 1.5k Pull-Up on USB D- Pin; Comment out if not used / tied to 3V3!

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB device descriptor
#define USB_VENDOR_ID       0x1209    // VID
#define USB_PRODUCT_ID      0xc003    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "MouseWiggler"
#define SERIAL_STR          "CH32V003HID"

