// ===================================================================================
// User configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_LED             P33       // pin connected to LED

// USB device descriptor
#define USB_VENDOR_ID       0x6666    // VID: Prototype
#define USB_PRODUCT_ID      0x6666    // PID: Prototype
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR    'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR         'C','H','5','5','x',' ','D','e','v','B','o','a','r','d'
#define SERIAL_STR          'C','H','5','5','x','C','D','C'
#define INTERFACE_STR       'C','D','C','-','S','e','r','i','a','l'
