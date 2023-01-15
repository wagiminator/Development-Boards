// ===================================================================================
// User configurations
// ===================================================================================

#pragma once

// Pin definitions

// USB device descriptor
#define USB_VENDOR_ID       0x6666    // VID: Prototype
#define USB_PRODUCT_ID      0x6666    // PID: Prototype
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUFACTURER_STR        'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR             'C','H','5','5','2','E',' ','D','e','v','S','t','i','c','k'
#define SERIAL_STR              'C','D','C'
#define INTERFACE_STR           'C','D','C','-','S','e','r','i','a','l'
