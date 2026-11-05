// ===================================================================================
// USB Descriptors and Definitions for CMSIS-DAP v2 Debug Probe
// ===================================================================================
//
// Definition of USB descriptors and endpoints of the composite device:
// Interface 0:    CMSIS-DAP v2 (vendor class, WinUSB)
//                 EP1 OUT: DAP commands, EP1 IN: DAP responses, EP2 IN: SWO trace
// Interface 1+2:  CDC-ACM virtual COM port (optional, UART_COM_PORT in config.h)
//                 EP3 IN: notifications, EP5 OUT/IN: data
//
// The following must be defined in config.h:
// USB_VENDOR_ID            - Vendor ID (16-bit word)
// USB_PRODUCT_ID           - Product ID (16-bit word)
// USB_DEVICE_VERSION       - Device version (16-bit BCD)
// USB_LANGUAGE             - Language descriptor code
// USB_MAX_POWER_mA         - Device max power in mA
// USB_WINUSB               - 1: provide Microsoft OS 2.0 descriptors (WinUSB)
// USB_MS_VENDOR_CODE       - vendor request code for the MS OS 2.0 descriptor
// All string descriptors.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "usb.h"
#include "config.h"

// ===================================================================================
// USB Endpoint Definitions
// ===================================================================================
#if (DAP_UART_ENABLE > 0) && (UART_COM_PORT > 0)
  #define USB_CDC_ENABLE  1               // CDC virtual COM port interfaces
#else
  #define USB_CDC_ENABLE  0
#endif

// Endpoint sizes
#define EP0_SIZE        64
#define EP1_SIZE        64                // DAP command/response
#define EP2_SIZE        64                // SWO trace
#define EP3_SIZE        8                 // CDC notification
#define EP5_SIZE        64                // CDC data

// Endpoint buffer sizes (endpoints with OUT and IN: OUT at offset 0, IN at offset 64)
#define EP0_BUF_SIZE    64
#define EP1_BUF_SIZE    128
#define EP2_BUF_SIZE    64
#define EP3_BUF_SIZE    64
#define EP5_BUF_SIZE    128

// Endpoint buffers
extern uint8_t __attribute__((aligned(4))) EP0_buffer[];
extern uint8_t __attribute__((aligned(4))) EP1_buffer[];
extern uint8_t __attribute__((aligned(4))) EP2_buffer[];
#if USB_CDC_ENABLE > 0
extern uint8_t __attribute__((aligned(4))) EP3_buffer[];
extern uint8_t __attribute__((aligned(4))) EP5_buffer[];
#endif

// ===================================================================================
// Device and Configuration Descriptors
// ===================================================================================
typedef struct __attribute__((packed)) {
  USB_CFG_DESCR  config;
  USB_ITF_DESCR  interface0;            // CMSIS-DAP v2
  USB_ENDP_DESCR ep1OUT;
  USB_ENDP_DESCR ep1IN;
  USB_ENDP_DESCR ep2IN;
  #if USB_CDC_ENABLE > 0
  USB_IAD_DESCR  association1;          // CDC interface association
  USB_ITF_DESCR  interface1;            // CDC control interface
  uint8_t        functional1[19];       // CDC functional descriptors
  USB_ENDP_DESCR ep3IN;
  USB_ITF_DESCR  interface2;            // CDC data interface
  USB_ENDP_DESCR ep5OUT;
  USB_ENDP_DESCR ep5IN;
  #endif
} USB_CFG_DESCR_DAP, *PUSB_CFG_DESCR_DAP;

extern const USB_DEV_DESCR DevDescr;
extern const USB_CFG_DESCR_DAP CfgDescr;

// ===================================================================================
// Microsoft OS 2.0 Descriptors (WinUSB for interface 0)
// ===================================================================================
#if USB_WINUSB > 0
#define USB_DESCR_TYP_BOS       0x0F
#define USB_BOS_DESCR_LEN       33
#if USB_CDC_ENABLE > 0
#define USB_MSOS20_SET_LEN      0xB2      // with configuration and function subset
#else
#define USB_MSOS20_SET_LEN      0xA2
#endif

extern const uint8_t BOSDescr[];
extern const uint8_t MSOS20Descr[];

#define USB_BOS_DESCR           (uint8_t*)BOSDescr
#endif

// ===================================================================================
// String Descriptors
// ===================================================================================
extern const USB_STR_DESCR LangDescr;
extern const USB_STR_DESCR ManufDescr;
extern const USB_STR_DESCR ProdDescr;
extern const USB_STR_DESCR DapInterfDescr;
extern const USB_STR_DESCR CdcInterfDescr;
extern uint8_t SerDescr[];

#define USB_STR_DESCR_i0    (uint8_t*)&LangDescr
#define USB_STR_DESCR_i1    (uint8_t*)&ManufDescr
#define USB_STR_DESCR_i2    (uint8_t*)&ProdDescr
#define USB_STR_DESCR_i3    (uint8_t*)SerDescr
#define USB_STR_DESCR_i4    (uint8_t*)&DapInterfDescr
#define USB_STR_DESCR_i5    (uint8_t*)&CdcInterfDescr
#define USB_STR_DESCR_ix    (uint8_t*)SerDescr

#define USB_CHAR_GLUE(s) u##s
#define USB_CHAR_TO_STR_DESCR(s)  {           \
  .bLength = sizeof(USB_CHAR_GLUE(s)),        \
  .bDescriptorType = USB_DESCR_TYP_STRING,    \
  .bString = USB_CHAR_GLUE(s)                 \
}

// Serial number string (from unique chip ID or SERIAL_STR)
void    USB_initSerial(void);
uint8_t USB_getSerialString(char *str);

#ifdef __cplusplus
}
#endif
