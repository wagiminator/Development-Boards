// ===================================================================================
// USB Descriptors for CMSIS-DAP v2 Debug Probe
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_descr.h"
#include "system.h"

#define __code const __attribute__((section(".rodata")))

#ifndef ESIG_UNIID
#define ESIG_UNIID          0x1FFFF7E8    // address of 96-bit unique ID
#endif

// ===================================================================================
// Endpoint Buffers
// ===================================================================================
uint8_t __attribute__((aligned(4))) EP0_buffer[EP0_BUF_SIZE];
uint8_t __attribute__((aligned(4))) EP1_buffer[EP1_BUF_SIZE];
uint8_t __attribute__((aligned(4))) EP2_buffer[EP2_BUF_SIZE];
#if USB_CDC_ENABLE > 0
uint8_t __attribute__((aligned(4))) EP3_buffer[EP3_BUF_SIZE];
uint8_t __attribute__((aligned(4))) EP5_buffer[EP5_BUF_SIZE];
#endif

// ===================================================================================
// Device Descriptor
// ===================================================================================
__code USB_DEV_DESCR DevDescr = {
  .bLength            = sizeof(DevDescr),       // size of the descriptor in bytes: 18
  .bDescriptorType    = USB_DESCR_TYP_DEVICE,   // device descriptor: 0x01
  #if USB_WINUSB > 0
  .bcdUSB             = 0x0210,                 // USB 2.1 (required for BOS descriptor)
  #else
  .bcdUSB             = 0x0200,                 // USB specification: USB 2.0
  #endif
  #if USB_CDC_ENABLE > 0
  .bDeviceClass       = USB_DEV_CLASS_MISC,     // composite device with IAD: 0xEF
  .bDeviceSubClass    = 0x02,                   // common class
  .bDeviceProtocol    = 0x01,                   // interface association descriptor
  #else
  .bDeviceClass       = 0,                      // interface will define class
  .bDeviceSubClass    = 0,                      // unused
  .bDeviceProtocol    = 0,                      // unused
  #endif
  .bMaxPacketSize0    = EP0_SIZE,               // maximum packet size for Endpoint 0
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
__code USB_CFG_DESCR_DAP CfgDescr = {

  // Configuration Descriptor
  .config = {
    .bLength            = sizeof(USB_CFG_DESCR),  // size of the descriptor in bytes
    .bDescriptorType    = USB_DESCR_TYP_CONFIG,   // configuration descriptor: 0x02
    .wTotalLength       = sizeof(CfgDescr),       // total length in bytes
    .bNumInterfaces     = 1 + 2*USB_CDC_ENABLE,   // number of interfaces: 1 or 3
    .bConfigurationValue= 1,                      // value to select this configuration
    .iConfiguration     = 0,                      // no configuration string descriptor
    .bmAttributes       = 0x80,                   // attributes = bus powered, no wakeup
    .MaxPower           = USB_MAX_POWER_mA / 2    // in 2mA units
  },

  // Interface Descriptor: Interface 0 (CMSIS-DAP v2)
  .interface0 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 0,                      // number of this interface: 0
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 3,                      // number of endpoints used: 3
    .bInterfaceClass    = USB_DEV_CLASS_VENDOR,   // interface class: vendor (0xff)
    .bInterfaceSubClass = 0x00,                   // unused
    .bInterfaceProtocol = 0x00,                   // unused
    .iInterface         = 4                       // index of String Descriptor ("CMSIS-DAP")
  },

  // Endpoint Descriptor: Endpoint 1 (OUT, DAP commands)
  .ep1OUT = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP1_OUT,  // endpoint: 1, direction: OUT (0x01)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP1_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  },

  // Endpoint Descriptor: Endpoint 1 (IN, DAP responses)
  .ep1IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP1_IN,   // endpoint: 1, direction: IN (0x81)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP1_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  },

  // Endpoint Descriptor: Endpoint 2 (IN, SWO trace data)
  .ep2IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP2_IN,   // endpoint: 2, direction: IN (0x82)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP2_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  },

  #if USB_CDC_ENABLE > 0
  // Interface Association Descriptor (CDC)
  .association1 = {
    .bLength            = sizeof(USB_IAD_DESCR),  // size of the descriptor in bytes: 8
    .bDescriptorType    = USB_DESCR_TYP_IAD,      // interface association descr: 0x0b
    .bFirstInterface    = 1,                      // first interface: 1
    .bInterfaceCount    = 2,                      // total number of interfaces: 2
    .bFunctionClass     = USB_DEV_CLASS_COMM,     // function class: CDC (0x02)
    .bFunctionSubClass  = 0x02,                   // function subclass: ACM
    .bFunctionProtocol  = 0x01,                   // function protocol: AT commands
    .iFunction          = 5                       // index of String Descriptor
  },

  // Interface Descriptor: Interface 1 (CDC control)
  .interface1 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 1,                      // number of this interface: 1
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 1,                      // number of endpoints used: 1
    .bInterfaceClass    = USB_DEV_CLASS_COMM,     // interface class: CDC (0x02)
    .bInterfaceSubClass = 0x02,                   // interface subclass: ACM
    .bInterfaceProtocol = 0x01,                   // interface protocol: AT commands
    .iInterface         = 5                       // index of String Descriptor
  },

  // CDC Functional Descriptors
  .functional1 = {
    0x05,0x24,0x00,0x10,0x01,                     // header functional descriptor (CDC 1.10)
    0x05,0x24,0x01,0x00,0x02,                     // call management functional descriptor
    0x04,0x24,0x02,0x06,                          // ACM: line coding, line state, break
    0x05,0x24,0x06,0x01,0x02                      // union functional descriptor: IF1, IF2
  },

  // Endpoint Descriptor: Endpoint 3 (IN, CDC notification)
  .ep3IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP3_IN,   // endpoint: 3, direction: IN (0x83)
    .bmAttributes       = USB_ENDP_TYPE_INTER,    // transfer type: interrupt (0x03)
    .wMaxPacketSize     = EP3_SIZE,               // max packet size
    .bInterval          = 16                      // polling intervall in ms
  },

  // Interface Descriptor: Interface 2 (CDC data)
  .interface2 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 2,                      // number of this interface: 2
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 2,                      // number of endpoints used: 2
    .bInterfaceClass    = USB_DEV_CLASS_DATA,     // interface class: data (0x0a)
    .bInterfaceSubClass = 0,                      // interface subclass
    .bInterfaceProtocol = 0,                      // interface protocol
    .iInterface         = 5                       // index of String Descriptor
  },

  // Endpoint Descriptor: Endpoint 5 (OUT, CDC data)
  .ep5OUT = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = 0x05,                   // endpoint: 5, direction: OUT (0x05)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP5_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  },

  // Endpoint Descriptor: Endpoint 5 (IN, CDC data)
  .ep5IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = 0x85,                   // endpoint: 5, direction: IN (0x85)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP5_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  }
  #endif
};

// ===================================================================================
// Microsoft OS 2.0 Descriptors (WinUSB for the CMSIS-DAP v2 interface)
// ===================================================================================
#if USB_WINUSB > 0

// Binary Device Object Store (BOS) Descriptor with MS OS 2.0 Platform Capability
__code uint8_t BOSDescr[USB_BOS_DESCR_LEN] = {
  0x05, USB_DESCR_TYP_BOS,                        // bLength, bDescriptorType: BOS
  USB_BOS_DESCR_LEN, 0x00,                        // wTotalLength
  0x01,                                           // bNumDeviceCaps
  // Platform Capability Descriptor
  0x1C, 0x10, 0x05, 0x00,                         // bLength, DEVICE CAP, PLATFORM, reserved
  0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C, // MS OS 2.0 Platform Capability UUID
  0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F, // {D8DD60DF-4589-4CC7-9CD2-659D9E648A9F}
  0x00, 0x00, 0x03, 0x06,                         // dwWindowsVersion: Windows 8.1+
  USB_MSOS20_SET_LEN, 0x00,                       // wMSOSDescriptorSetTotalLength
  USB_MS_VENDOR_CODE,                             // bMS_VendorCode
  0x00                                            // bAltEnumCode
};

// MS OS 2.0 Descriptor Set (WinUSB compatible ID and CMSIS-DAP v2 interface GUID)
__code uint8_t MSOS20Descr[USB_MSOS20_SET_LEN] = {
  // Descriptor set header
  0x0A, 0x00, 0x00, 0x00,                         // wLength, MS_OS_20_SET_HEADER_DESCRIPTOR
  0x00, 0x00, 0x03, 0x06,                         // dwWindowsVersion: Windows 8.1+
  USB_MSOS20_SET_LEN, 0x00,                       // wTotalLength
  #if USB_CDC_ENABLE > 0
  // Configuration subset header (composite device)
  0x08, 0x00, 0x01, 0x00,                         // wLength, MS_OS_20_SUBSET_HEADER_CONFIGURATION
  0x00, 0x00,                                     // bConfigurationValue (index 0), reserved
  0xA8, 0x00,                                     // wTotalLength of this subset
  // Function subset header (interface 0)
  0x08, 0x00, 0x02, 0x00,                         // wLength, MS_OS_20_SUBSET_HEADER_FUNCTION
  0x00, 0x00,                                     // bFirstInterface: 0, reserved
  0xA0, 0x00,                                     // wSubsetLength
  #endif
  // Compatible ID descriptor
  0x14, 0x00, 0x03, 0x00,                         // wLength, MS_OS_20_FEATURE_COMPATIBLE_ID
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,       // CompatibleID: "WINUSB"
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // SubCompatibleID
  // Registry property descriptor
  0x84, 0x00, 0x04, 0x00,                         // wLength, MS_OS_20_FEATURE_REG_PROPERTY
  0x07, 0x00,                                     // wPropertyDataType: REG_MULTI_SZ
  0x2A, 0x00,                                     // wPropertyNameLength: 42
  'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,'t',0,'e',0,'r',0,
  'f',0,'a',0,'c',0,'e',0,'G',0,'U',0,'I',0,'D',0,'s',0,0x00,0x00,
  0x50, 0x00,                                     // wPropertyDataLength: 80
  '{',0,'C',0,'D',0,'B',0,'3',0,'B',0,'5',0,'A',0,'D',0,'-',0,    // CMSIS-DAP v2 GUID
  '2',0,'9',0,'3',0,'B',0,'-',0,'4',0,'6',0,'6',0,'3',0,'-',0,    // (as DAPLink)
  'A',0,'A',0,'3',0,'6',0,'-',0,'1',0,'A',0,'A',0,'E',0,'4',0,
  '6',0,'4',0,'6',0,'3',0,'7',0,'7',0,'6',0,'}',0,0x00,0x00,0x00,0x00
};

#endif

// ===================================================================================
// String Descriptors
// ===================================================================================

// Language Descriptor (Index 0)
__code USB_STR_DESCR LangDescr = {
  .bLength              = 4,
  .bDescriptorType      = USB_DESCR_TYP_STRING,
  .bString              = {USB_LANGUAGE}
};

// Manufacturer String Descriptor (Index 1)
__code USB_STR_DESCR ManufDescr = USB_CHAR_TO_STR_DESCR(MANUF_STR);

// Product String Descriptor (Index 2)
__code USB_STR_DESCR ProdDescr = USB_CHAR_TO_STR_DESCR(PROD_STR);

// CMSIS-DAP Interface String Descriptor (Index 4)
__code USB_STR_DESCR DapInterfDescr = USB_CHAR_TO_STR_DESCR(DAP_INTERF_STR);

// CDC Interface String Descriptor (Index 5)
__code USB_STR_DESCR CdcInterfDescr = USB_CHAR_TO_STR_DESCR(CDC_INTERF_STR);

// Serial String Descriptor (Index 3, built at startup)
#define USB_SERIAL_MAXLEN   24
uint8_t __attribute__((aligned(2))) SerDescr[2 + 2*USB_SERIAL_MAXLEN];

// Build serial number string descriptor
void USB_initSerial(void) {
  uint8_t i, len = 0;
  #if USB_SERIAL_UID > 0
  // 96-bit unique ID as 24 hex digits
  static const char hex[] = "0123456789ABCDEF";
  const uint32_t* uid = (const uint32_t*)ESIG_UNIID;
  for(i=0; i<24; i++) {
    uint32_t w = uid[i >> 3];
    SerDescr[2 + 2*i] = hex[(w >> (28 - ((i & 7) << 2))) & 0x0F];
    SerDescr[3 + 2*i] = 0;
  }
  len = 24;
  #else
  static const char ser[] = SERIAL_STR;
  for(i=0; ser[i] && (i < USB_SERIAL_MAXLEN); i++) {
    SerDescr[2 + 2*i] = ser[i];
    SerDescr[3 + 2*i] = 0;
  }
  len = i;
  #endif
  SerDescr[0] = 2 + 2*len;
  SerDescr[1] = USB_DESCR_TYP_STRING;
}

// Copy serial number as ASCII string, returns length including terminating NULL
uint8_t USB_getSerialString(char *str) {
  uint8_t i, len = (SerDescr[0] - 2) >> 1;
  for(i=0; i<len; i++) str[i] = SerDescr[2 + 2*i];
  str[len] = 0;
  return len + 1;
}
