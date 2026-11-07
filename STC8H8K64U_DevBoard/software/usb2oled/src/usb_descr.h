// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================
//
// Definition of USB descriptors and endpoint sizes and addresses.
//
// The following must be defined in config.h:
// USB_VENDOR_ID            - Vendor ID (16-bit word)
// USB_PRODUCT_ID           - Product ID (16-bit word)
// USB_DEVICE_VERSION       - Device version (16-bit BCD)
// USB_MAX_POWER_mA         - Device max power in mA
// All string descriptors.

#pragma once
#include <stdint.h>
#include "usb.h"
#include "config.h"

// ===================================================================================
// USB Endpoint Definitions
// ===================================================================================
#define EP0_SIZE        64
#define EP1_SIZE        8
#define EP2_SIZE        64

#define EP0_BUF_SIZE    EP0_SIZE
#define EP1_BUF_SIZE    EP1_SIZE
#define EP2_BUF_SIZE    128             // OUT: 0..63, IN: 64..127

extern __xdata uint8_t EP0_buffer[EP0_BUF_SIZE];
extern __xdata uint8_t EP1_buffer[EP1_BUF_SIZE];
extern __xdata uint8_t EP2_buffer[EP2_BUF_SIZE];

// ===================================================================================
// Custom External USB Handler Functions and Defines
// ===================================================================================
// External USB handler functions
uint8_t CDC_control(void);
void CDC_EP_init(void);
void CDC_EP0_OUT(void);
void CDC_EP2_IN(void);
void CDC_EP2_OUT(void);

// Custom USB handler functions
#define USB_INIT_endpoints      CDC_EP_init     // custom USB EP init handler
#define USB_CLASS_SETUP_handler CDC_control     // handle class setup requests
#define USB_CLASS_OUT_handler   CDC_EP0_OUT     // handle class out transfers

// Endpoint callback functions
#define EP2_IN_callback         CDC_EP2_IN
#define EP2_OUT_callback        CDC_EP2_OUT

// ===================================================================================
// Device and Configuration Descriptors
// ===================================================================================
typedef struct _USB_CFG_DESCR_CDC {
  USB_CFG_DESCR config;
  USB_IAD_DESCR association;
  USB_ITF_DESCR interface0;
  uint8_t functional[19];
  USB_ENDP_DESCR ep1IN;
  USB_ITF_DESCR interface1;
  USB_ENDP_DESCR ep2OUT;
  USB_ENDP_DESCR ep2IN;
} USB_CFG_DESCR_CDC, *PUSB_CFG_DESCR_CDC;
typedef USB_CFG_DESCR_CDC __xdata *PXUSB_CFG_DESCR_CDC;

extern __code USB_DEV_DESCR DevDescr;
extern __code USB_CFG_DESCR_CDC CfgDescr;

// ===================================================================================
// String Descriptors
// ===================================================================================
extern __code uint16_t LangDescr[];
extern __code uint16_t ManufDescr[];
extern __code uint16_t ProdDescr[];
extern __code uint16_t SerDescr[];
extern __code uint16_t InterfDescr[];

#define USB_STR_DESCR_i0    (uint8_t*)LangDescr
#define USB_STR_DESCR_i1    (uint8_t*)ManufDescr
#define USB_STR_DESCR_i2    (uint8_t*)ProdDescr
#define USB_STR_DESCR_i3    (uint8_t*)SerDescr
#define USB_STR_DESCR_i4    (uint8_t*)InterfDescr
#define USB_STR_DESCR_ix    (uint8_t*)SerDescr
