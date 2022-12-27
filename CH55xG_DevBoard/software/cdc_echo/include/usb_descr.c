// ===================================================================================
// USB Descriptors
// ===================================================================================

#include "usb_descr.h"

// ===================================================================================
// Device Descriptor
// ===================================================================================
__code USB_DEV_DESCR DevDescr = {
  .bLength            = sizeof(DevDescr),       // size of the descriptor in bytes: 18
  .bDescriptorType    = USB_DESCR_TYP_DEVICE,   // device descriptor: 0x01
  .bcdUSB             = 0x0200,                 // USB specification: USB 2.0
  .bDeviceClass       = 0,                      // interface will define class
  .bDeviceSubClass    = 0,                      // unused
  .bDeviceProtocol    = 0,                      // unused
  .bMaxPacketSize0    = EP0_SIZE,               // maximum packet size for Endpoint 0
  .idVendor           = VENDOR_ID,              // VID
  .idProduct          = PRODUCT_ID,             // PID
  .bcdDevice          = DEVICE_VERSION,         // device version
  .iManufacturer      = 1,                      // index of Manufacturer String Descr
  .iProduct           = 2,                      // index of Product String Descriptor
  .iSerialNumber      = 3,                      // index of Serial Number String Descr
  .bNumConfigurations = 1                       // number of possible configurations
};

// ===================================================================================
// Configuration Descriptor
// ===================================================================================
__code USB_CFG_DESCR_CDC CfgDescr = {

  // Configuration Descriptor
  .config = {
    .bLength            = sizeof(USB_CFG_DESCR),  // size of the descriptor in bytes
    .bDescriptorType    = USB_DESCR_TYP_CONFIG,   // configuration descriptor: 0x02
    .wTotalLength       = sizeof(CfgDescr),       // total length in bytes
    .bNumInterfaces     = 2,                      // number of interfaces: 2
    .bConfigurationValue= 1,                      // value to select this configuration
    .iConfiguration     = 4,                      // index of String Descr for this config
    .bmAttributes       = 0x80,                   // attributes = bus powered, no wakeup
    .MaxPower           = 100                     // in 2mA units = 200mA
  },

  // Interface Descriptor: Interface 0 (CDC)
  .interface0 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 0,                      // number of this interface: 0
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 1,                      // number of endpoints used: 1
    .bInterfaceClass    = USB_DEV_CLASS_COMM,     // interface class: CDC (0x02)
    .bInterfaceSubClass = 2,                      // interface sub class
    .bInterfaceProtocol = 1,                      // interface protocol
    .iInterface         = 4                       // index of String Descriptor
  },

  // Functional Descriptors for Interface 0
  .functional = {
    0x05,0x24,0x00,0x10,0x01,                     // header functional descriptor
    0x05,0x24,0x01,0x00,0x01,                     // call management functional descriptor
    0x04,0x24,0x02,0x02,                          // direct line management functional descriptor
    0x05,0x24,0x06,0x00,0x01                      // union functional descriptor: CDC IF0, Data IF1
  },

  // Endpoint Descriptor: Endpoint 1 (CDC Upload, Interrupt)
  .ep1IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = 0x81,                   // endpoint: 1, direction: IN
    .bmAttributes       = USB_ENDP_TYPE_INTER,    // transfer type: interrupt (0x03)
    .wMaxPacketSize     = EP1_SIZE,               // max packet size: 8
    .bInterval          = 0x02                    // polling intervall
  },

  // Interface Descriptor: Interface 1 (Data)
  .interface1 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 1,                      // number of this interface: 1
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 2,                      // number of endpoints used: 2
    .bInterfaceClass    = USB_DEV_CLASS_DATA,     // interface class: data (0x0a)
    .bInterfaceSubClass = 0,                      // interface sub class
    .bInterfaceProtocol = 0,                      // interface protocol
    .iInterface         = 4                       // index of String Descriptor
  },

  // Endpoint Descriptor: Endpoint 2 (OUT)
  .ep2OUT = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = 0x02,                   // endpoint: 2, direction: OUT
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP2_SIZE,               // max packet size: 64
    .bInterval          = 0x00                    // polling intervall (ignored for bulk)
  },

  // Endpoint Descriptor: Endpoint 2 (IN)
  .ep2IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = 0x82,                   // endpoint: 2, direction: IN
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP2_SIZE,               // max packet size: 64
    .bInterval          = 0x00                    // polling intervall (ignored for bulk)
  }
};

// ===================================================================================
// String Descriptors
// ===================================================================================

// Language Descriptor (Index 0)
__code uint16_t LangDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(LangDescr), 0x0409   // US English
};

// Manufacturer String Descriptor (Index 1)
__code uint16_t ManufDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ManufDescr), MANUFACTURER_STR
};

// Product String Descriptor (Index 2)
__code uint16_t ProdDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ProdDescr), PRODUCT_STR
};

// Serial String Descriptor (Index 3)
__code uint16_t SerDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(SerDescr), SERIAL_STR
};

// CDC String Descriptor (Index 4)
__code uint16_t CDC_Descr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(CDC_Descr), INTERFACE_STR
};
