// ===================================================================================
// User Configurations for CH55xE Development Stick
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_NEO             P14       // pin connected to NeoPixel
#define PIN_LED             P15       // pin connected to LED
#define PIN_TOUCH           P16       // pin connected to touch key
#define PIN_ACTKEY          P17       // pin connected to ACT-button
#define LED_BUILTIN         P15       // builtin LED

// NeoPixel configuration
#define NEO_GRB                       // type of pixel: NEO_GRB or NEO_RGB

// Touchkey configuration
#define TOUCH_TH_LOW        1800      // key pressed threshold
#define TOUCH_TH_HIGH       2200      // key released threshold

// USB device descriptor
#define USB_VENDOR_ID       0x6666    // VID: Prototype
#define USB_PRODUCT_ID      0x6666    // PID: Prototype
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'C','H','5','5','x','E',' ','D','e','v','S','t','i','c','k'
#define SERIAL_STR          'C','D','C'
#define INTERFACE_STR       'C','D','C','-','S','e','r','i','a','l'
