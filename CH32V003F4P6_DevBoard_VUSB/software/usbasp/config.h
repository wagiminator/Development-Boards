// ===================================================================================
// USBasp User Configurations
// ===================================================================================
//
// Description:
// ------------
// USBasp-compatible ISP/TPI programmer for AVR microcontrollers. It works with
// AVRDUDE (avrdude -c usbasp) and the Arduino IDE (Tools -> Programmer -> USBasp).
//
// Operating Instructions:
// -----------------------
// - Connect the target board to the pins defined below (and to GND).
// - The CH32V003 runs at 3.3V, so the target must be supplied with 3.3V or a
//   level shifter must be used!
// - Connect the board via USB to your PC. It should be detected as USBasp.
// - Run AVRDUDE, e.g. 'avrdude -c usbasp -p m328p -U flash:w:firmware.hex'.

#pragma once

// Pin definitions for ISP/TPI bus
#define PIN_RESET           PC4       // pin connected to ICSP RESET
#define PIN_SCK             PC5       // pin connected to ICSP SCK  / TPI CLK  (do not change)
#define PIN_MOSI            PC6       // pin connected to ICSP MOSI / TPI DATA (do not change)
#define PIN_MISO            PC7       // pin connected to ICSP MISO            (do not change)

// Pin definitions for status LEDs (active LOW, comment out if not used)
#define PIN_LED_PRG         PC0       // pin connected to PROGRAMMING status LED
#define PIN_LED_USB         PD0       // pin connected to USB status LED

// USB pin definitions
#define USB_PORT            A         // [A,C,D] GPIO Port to use with D+, D- and DPU
#define USB_PIN_DP          1         // [0-4] GPIO Number for USB D+ Pin
#define USB_PIN_DM          2         // [0-4] GPIO Number for USB D- Pin
//#define USB_PIN_DPU         5         // [0-7] GPIO for feeding the 1.5k Pull-Up on USB D- Pin; Comment out if not used / tied to 3V3!

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA (including target board)

// USB device descriptor (VID/PID and strings must not be changed for AVRDUDE)
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x05DC    // PID (shared vendor class with libusb)
#define USB_DEVICE_VERSION  0x0108    // v1.08 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB descriptor strings
#define MANUF_STR           "www.fischl.de"
#define PROD_STR            "USBasp"
#define SERIAL_STR          "CH32V003ASP"

// Windows Compatible ID (WCID) code for automated driver installation.
// Comment out this define to disable the WCID feature.
#define WCID_VENDOR_CODE    0x5D
