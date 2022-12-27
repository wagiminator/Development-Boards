// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================

#pragma once

#include <stdint.h>
#include "ch554.h"
#include "usb.h"

// ===================================================================================
// USB Defines
// ===================================================================================
#define EP0_SIZE                DEFAULT_ENDP0_SIZE
#define EP1_SIZE                DEFAULT_ENDP1_SIZE
#define EP2_SIZE                MAX_PACKET_SIZE

#define EP0_ADDR                 0
#define EP1_ADDR                (EP0_ADDR + EP0_SIZE + 2)
#define EP2_ADDR                (EP1_ADDR + EP1_SIZE + 2)

#define SET_LINE_CODING         0X20  // configures DTE rate, stop-bits, parity, and number-of-character
#define GET_LINE_CODING         0X21  // allows the host to find out the currently configured line coding
#define SET_CONTROL_LINE_STATE  0X22  // generates RS-232/V.24 style control signals

// ===================================================================================
// IDs and Descriptor Strings
// ===================================================================================
#define VENDOR_ID               0x6666    // VID: Prototype
#define PRODUCT_ID              0x6666    // PID: Prototype
#define DEVICE_VERSION          0x0100    // v1.0 (BCD-format)

#define MANUFACTURER_STR        'w','a','g','i','m','i','n','a','t','o','r'
#define PRODUCT_STR             'C','H','5','5','x',' ','D','e','v','B','o','a','r','d'
#define SERIAL_STR              'C','D','C'
#define INTERFACE_STR           'C','D','C',' ','S','e','r','i','a','l'

// ===================================================================================
// USB Descriptors
// ===================================================================================
typedef struct _USB_CONFIG_DESCR_CDC {
  USB_CFG_DESCR config;
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
extern __code uint16_t LangDescr[];
extern __code uint16_t SerDescr[];
extern __code uint16_t ProdDescr[];
extern __code uint16_t CDC_Descr[];
extern __code uint16_t ManufDescr[];
