// ===================================================================================
// User Configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_LED             PA5       // pin connected to LED

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "SAMD11C14A-CDC"
#define SERIAL_STR          "SAMD11CDC"
#define INTERF_STR          "CDC-Serial"
