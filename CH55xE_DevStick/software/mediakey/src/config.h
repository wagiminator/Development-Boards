// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_LED             P15       // pin connected to LED
#define PIN_NEO             P14       // pin connected to NeoPixel
#define PIN_TOUCH           P16       // pin connected to touch key
#define PIN_ACTKEY          P17       // pin connected to ACT button
#define NEO_GRB                       // type of pixel: NEO_GRB or NEO_RGB
#define NEO_COUNT           1         // number of pixels
#define TOUCH_TH_LOW        2000      // key pressed threshold
#define TOUCH_TH_HIGH       2400      // key released threshold

// USB device descriptor
#define USB_VENDOR_ID       0x1189    // VID
#define USB_PRODUCT_ID      0x8890    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'C','H','5','5','x','C','o','m','p','o','s','i','t','e'
#define SERIAL_STR          'C','H','5','5','x','H','I','D'
#define INTERFACE_STR       'H','I','D','-','C','o','m','p','o','s','i','t','e'
