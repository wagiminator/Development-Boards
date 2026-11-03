// ===================================================================================
// DAPLink User Configurations
// ===================================================================================
//
// Description:
// ------------
// CMSIS-DAP v1 (HID) compliant debug probe with SWD protocol support. It works with
// tinyOCD, OpenOCD, pyOCD and other software that supports CMSIS-DAP.
//
// Operating Instructions:
// -----------------------
// - Connect the target board to the pins defined below (and to GND).
// - The CH32V003 runs at 3.3V, so the target must run on 3.3V or a level shifter
//   must be used!
// - Connect the board via USB to your PC. It is detected as a HID device, no driver
//   is needed.
// - Run your debug software, e.g. 'python3 tinyocd.py -f firmware.bin'.

#pragma once

// Pin definitions for SWD bus
#define PIN_SWCLK           PC5       // pin connected to target SWCLK
#define PIN_SWDIO           PC6       // pin connected to target SWDIO
#define PIN_NRST            PC4       // pin connected to target nRESET (open-drain)

// Pin definitions for status LEDs (active LOW, comment out if not used)
#define PIN_LED_CON         PC0       // pin connected to CONNECTED status LED
#define PIN_LED_RUN         PD0       // pin connected to TARGET RUNNING status LED

// CMSIS-DAP settings
#define DAP_DEFAULT_CLOCK   1000000   // default SWD clock in Hz (max. approx. 1.1 MHz)
#define DAP_SHORT_REPORTS   0         // 1: send only the used part of the 64-byte
                                      //    response (about 2x faster with tinyOCD/
                                      //    libusb, but not HID compliant)

// USB pin definitions
#define USB_PORT            A         // [A,C,D] GPIO Port to use with D+, D- and DPU
#define USB_PIN_DP          1         // [0-4] GPIO Number for USB D+ Pin
#define USB_PIN_DM          2         // [0-4] GPIO Number for USB D- Pin
//#define USB_PIN_DPU         5         // [0-7] GPIO for feeding the 1.5k Pull-Up on USB D- Pin; Comment out if not used / tied to 3V3!

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA (including target board)
#define USB_EP1_INTERVAL    1         // polling interval of the HID IN endpoint in ms

// USB device descriptor
#define USB_VENDOR_ID       0x1209    // VID
#define USB_PRODUCT_ID      0xC55D    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB descriptor strings (product or interface must contain "CMSIS-DAP")
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH32V003 CMSIS-DAP"
#define SERIAL_STR          "CH32V003DAP"
#define INTERF_STR          "CMSIS-DAP"
