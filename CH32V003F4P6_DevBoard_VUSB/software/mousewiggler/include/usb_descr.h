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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>

// ===================================================================================
// RV003USB Defines
// ===================================================================================
//Defines the number of endpoints for this device. (Always add one for EP0). For two EPs, this should be 3.
#define ENDPOINTS 2

#define RV003USB_OPTIMIZE_FLASH 1
#define RV003USB_HANDLE_IN_REQUEST 1
#define RV003USB_OTHER_CONTROL 0
#define RV003USB_HANDLE_USER_DATA 0
#define RV003USB_HID_FEATURES 0

#ifndef __ASSEMBLER__
#include <usb.h>

// ===================================================================================
// HID Report Descriptor
// ===================================================================================
static const uint8_t ReportDescr[] = {
  0x05, 0x01,     // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,     // USAGE (Mouse)
  0xa1, 0x01,     // COLLECTION (Application)
  0x09, 0x01,     //   USAGE (Pointer)
  0xa1, 0x00,     //   COLLECTION (Physical)
  0x05, 0x09,     //     USAGE_PAGE (Button)
  0x19, 0x01,     //     USAGE_MINIMUM (Button 1)
  0x29, 0x03,     //     USAGE_MAXIMUM (Button 3)
  0x15, 0x00,     //     LOGICAL_MINIMUM (0)
  0x25, 0x01,     //     LOGICAL_MAXIMUM (1)
  0x75, 0x01,     //     REPORT_SIZE (1)
  0x95, 0x03,     //     REPORT_COUNT (3)
  0x81, 0x02,     //     INPUT (Data,Var,Abs)
  0x75, 0x05,     //     REPORT_SIZE (5)
  0x95, 0x01,     //     REPORT_COUNT (1)
  0x81, 0x03,     //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,     //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,     //     USAGE (X)
  0x09, 0x31,     //     USAGE (Y)
  0x09, 0x38,     //     USAGE (Wheel)
  0x15, 0x81,     //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,     //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,     //     REPORT_SIZE (8)
  0x95, 0x03,     //     REPORT_COUNT (3)
  0x81, 0x06,     //     INPUT (Data,Var,Rel)
  0xc0,           //   END_COLLECTION
  0xc0            // END_COLLECTION
};

// ===================================================================================
// Device Descriptor
// ===================================================================================
static const USB_DEV_DESCR DevDescr = {
  .bLength            = sizeof(DevDescr),       // size of the descriptor in bytes: 18
  .bDescriptorType    = USB_DESCR_TYP_DEVICE,   // device descriptor: 0x01
  .bcdUSB             = 0x0110,                 // USB specification: USB 1.1
  .bDeviceClass       = 0,                      // interface will define class
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
struct USB_CFG_DESCR_HID {
  USB_CFG_DESCR config;
  USB_ITF_DESCR interface0;
  USB_HID_DESCR hid0;
  USB_ENDP_DESCR ep1IN;
};

static const struct USB_CFG_DESCR_HID CfgDescr = {
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

  // Interface Descriptor
  .interface0 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 0,                      // number of this interface: 0
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 1,                      // number of endpoints used: 1
    .bInterfaceClass    = USB_DEV_CLASS_HID,      // interface class: HID (0x03)
    .bInterfaceSubClass = 1,                      // boot interface
    .bInterfaceProtocol = 2,                      // mouse
    .iInterface         = 0                       // interface string descriptor
  },

  // HID Descriptor
  .hid0 = {
    .bLength            = sizeof(USB_HID_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_HID,      // HID descriptor: 0x21
    .bcdHID             = 0x0111,                 // HID class spec version (BCD: 1.11)
    .bCountryCode       = 0,                      // not supported
    .bNumDescriptors    = 1,                      // number of report descriptors: 1
    .bDescriptorTypeX   = USB_DESCR_TYP_REPORT,   // descriptor type: report (0x22)
    .wDescriptorLength  = sizeof(ReportDescr)     // report descriptor length
  },

  // Endpoint Descriptor: Endpoint 1 (IN, Interrupt)
  .ep1IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP1_IN,   // endpoint: 1, direction: IN (0x81)
    .bmAttributes       = USB_ENDP_TYPE_INTER,    // transfer type: interrupt (0x03)
    .wMaxPacketSize     = 4,                      // max packet size
    .bInterval          = 10                      // polling intervall in ms
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
  {0x00002200, ReportDescr, sizeof(ReportDescr)},
  {0x00000300, (const uint8_t *)&string0, 4},
  {0x04090301, (const uint8_t *)&string1, USB_CHAR_SIZE(MANUF_STR)},
  {0x04090302, (const uint8_t *)&string2, USB_CHAR_SIZE(PROD_STR)},	
  {0x04090303, (const uint8_t *)&string3, USB_CHAR_SIZE(SERIAL_STR)}
};

#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )

#endif  // __ASSEMBLER__

#ifdef __cplusplus
}
#endif
