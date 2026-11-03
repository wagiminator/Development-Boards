// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================
//
// Definition of USB descriptors and endpoints.
//
// The following must be defined in config.h:
// USB_VENDOR_ID            - Vendor ID (16-bit word)
// USB_PRODUCT_ID           - Product ID (16-bit word)
// USB_DEVICE_VERSION       - Device version (16-bit BCD)
// USB_LANGUAGE             - Language descriptor code
// USB_MAX_POWER_mA         - Device max power in mA
// All string descriptors.
// WCID_VENDOR_CODE         - Vendor code for Windows WCID (optional)

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

// ===================================================================================
// RV003USB Defines
// ===================================================================================
// Defines the number of endpoints for this device. (Always add one for EP0).
// The USBasp only uses the control endpoint EP0.
#define ENDPOINTS                     1

// Endpoint handling options
#define RV003USB_OPTIMIZE_FLASH       1
#define RV003USB_HANDLE_IN_REQUEST    1
#define RV003USB_OTHER_CONTROL        1
#define RV003USB_HANDLE_USER_DATA     1
#define RV003USB_HID_FEATURES         0
//#define RV003USB_SUPPORT_CONTROL_OUT  0
//#define RV003USB_CUSTOM_C             0

#ifndef __ASSEMBLER__
#include <usb.h>

// ===================================================================================
// Device Descriptor
// ===================================================================================
static const USB_DEV_DESCR DevDescr = {
  .bLength            = sizeof(DevDescr),       // size of the descriptor in bytes: 18
  .bDescriptorType    = USB_DESCR_TYP_DEVICE,   // device descriptor: 0x01
  #ifdef WCID_VENDOR_CODE
  .bcdUSB             = 0x0200,                 // USB 2.0 (needed for Windows OS descriptors)
  #else
  .bcdUSB             = 0x0110,                 // USB specification: USB 1.1
  #endif
  .bDeviceClass       = USB_DEV_CLASS_VENDOR,   // vendor specific class: 0xFF
  .bDeviceSubClass    = 0,                      // unused
  .bDeviceProtocol    = 0,                      // unused
  .bMaxPacketSize0    = 8,                      // maximum packet size for Endpoint 0
  .idVendor           = USB_VENDOR_ID,          // VID
  .idProduct          = USB_PRODUCT_ID,         // PID
  .bcdDevice          = USB_DEVICE_VERSION,     // device version
  .iManufacturer      = 1,                      // index of Manufacturer String Descr
  .iProduct           = 2,                      // index of Product String Descriptor
  .iSerialNumber      = 3,                      // index of Serial Number String Descr
  .bNumConfigurations = 1                       // number of possible configurations
};

// ===================================================================================
// Configuration Descriptor
// ===================================================================================
struct USB_CFG_DESCR_ASP {
  USB_CFG_DESCR config;
  USB_ITF_DESCR interface0;
};

static const struct USB_CFG_DESCR_ASP CfgDescr = {
  // Configuration Descriptor
  .config = {
    .bLength            = sizeof(USB_CFG_DESCR),  // size of the descriptor in bytes
    .bDescriptorType    = USB_DESCR_TYP_CONFIG,   // configuration descriptor: 0x02
    .wTotalLength       = sizeof(CfgDescr),       // total length in bytes
    .bNumInterfaces     = 1,                      // number of interfaces: 1
    .bConfigurationValue= 1,                      // value to select this configuration
    .iConfiguration     = 0,                      // no configuration string descriptor
    .bmAttributes       = 0x80,                   // attributes = bus powered, no wakeup
    .MaxPower           = USB_MAX_POWER_mA / 2    // in 2mA units
  },

  // Interface Descriptor (no endpoints besides EP0)
  .interface0 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 0,                      // number of this interface: 0
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 0,                      // number of endpoints used: 0
    .bInterfaceClass    = 0,                      // unused
    .bInterfaceSubClass = 0,                      // unused
    .bInterfaceProtocol = 0,                      // unused
    .iInterface         = 0                       // no interface string descriptor
  }
};

// ===================================================================================
// String Descriptors
// ===================================================================================
#define __code const __attribute__((section(".rodata")))
#define USB_CHAR_GLUE(s) u##s
#define USB_CHAR_SIZE(s) sizeof(USB_CHAR_GLUE(s))
#define USB_CHAR_TO_STR_DESCR(s)  {           \
  .bLength = sizeof(USB_CHAR_GLUE(s)),        \
  .bDescriptorType = USB_DESCR_TYP_STRING,    \
  .bString = USB_CHAR_GLUE(s)                 \
}

// Language Descriptor (Index 0)
static __code USB_STR_DESCR string0 = {
  .bLength              = 4,
  .bDescriptorType      = USB_DESCR_TYP_STRING,
  .bString              = {USB_LANGUAGE}
};

// Manufacturer String Descriptor (Index 1)
static __code USB_STR_DESCR string1 = USB_CHAR_TO_STR_DESCR(MANUF_STR);

// Product String Descriptor (Index 2)
static __code USB_STR_DESCR string2 = USB_CHAR_TO_STR_DESCR(PROD_STR);

// Serial String Descriptor (Index 3)
static __code USB_STR_DESCR string3 = USB_CHAR_TO_STR_DESCR(SERIAL_STR);

// ===================================================================================
// Windows Compatible ID (WCID) Descriptors for automated driver installation
// ===================================================================================
#ifdef WCID_VENDOR_CODE
// Microsoft OS String Descriptor (Index 0xEE): "MSFT100" + vendor code
static __code uint8_t WCID_STRING_DESCR[] = {
  0x12, USB_DESCR_TYP_STRING,                     // length = 18 bytes, string descriptor
  'M',0, 'S',0, 'F',0, 'T',0, '1',0, '0',0, '0',0, // signature "MSFT100" (unicode)
  WCID_VENDOR_CODE,                               // vendor code for feature descriptors
  0x00                                            // padding
};

// Microsoft Extended Compat ID Feature Descriptor (wIndex = 0x0004)
static __code uint8_t WCID_FEATURE_DESCR[] = {
  0x28, 0x00, 0x00, 0x00,                         // length = 40 bytes
  0x00, 0x01,                                     // version 1.0 (in BCD)
  0x04, 0x00,                                     // compatibility descriptor index 0x0004
  0x01,                                           // number of sections
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       // reserved (7 bytes)
  0x00,                                           // interface number 0
  0x01,                                           // reserved
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,       // compatible ID "WINUSB\0\0"
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // subcompatible ID (unused)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00              // reserved (6 bytes)
};
#endif

// ===================================================================================
// Descriptor Table
// ===================================================================================
// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
struct descriptor_list_struct {
  uint32_t      lIndexValue;
  const uint8_t *addr;
  uint8_t		    length;
};

const static struct descriptor_list_struct descriptor_list[] = {
  {0x00000100, (const uint8_t *)&DevDescr, sizeof(DevDescr)},
  {0x00000200, (const uint8_t *)&CfgDescr, sizeof(CfgDescr)},
  {0x00000300, (const uint8_t *)&string0, 4},
  {0x04090301, (const uint8_t *)&string1, USB_CHAR_SIZE(MANUF_STR)},
  {0x04090302, (const uint8_t *)&string2, USB_CHAR_SIZE(PROD_STR)},
  {0x04090303, (const uint8_t *)&string3, USB_CHAR_SIZE(SERIAL_STR)},
  #ifdef WCID_VENDOR_CODE
  {0x000003EE, WCID_STRING_DESCR, sizeof(WCID_STRING_DESCR)}
  #endif
};

#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )

#endif  // __ASSEMBLER__

#ifdef __cplusplus
}
#endif
