// ===================================================================================
// Rubber Ducky User Configurations
// ===================================================================================
//
// Description:
// ------------
// Identifies itself as a USB HID keyboard and types a message on button press.
//
// Operating Instructions:
// -----------------------
// - Connect a push button to the MCU pin defined below (active low).
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Open a text editor and place the cursor inside the text box.
// - Press the button and you can see a message typed in by the device.

#pragma once

// GPIO pin defines
#define PIN_LED             PC0       // pin connected to LED (active low)
#define PIN_KEY             PD0       // pin connected to push button (active low)

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
#define PROD_STR            "RubberDucky"
#define SERIAL_STR          "CH32V003HID"

