// ===================================================================================
// USB Descriptors and Definitions
// ===================================================================================

#pragma once
#include <stdint.h>
#include "usb.h"

// ===================================================================================
// USB Endpoint Addresses and Sizes
// ===================================================================================
#define EP0_SIZE        64
#define EP1_SIZE        16
#define EP2_SIZE        64

#define EP0_ADDR        0
#define EP1_ADDR        (EP0_ADDR + EP_ADDR_SIZE(EP0_SIZE))
#define EP2_ADDR        (EP1_ADDR + EP_ADDR_SIZE(EP1_SIZE))

#define EP_ADDR_SIZE(x) (x+2<64 ? x+2 : 64)

// ===================================================================================
// Device and Configuration Descriptors
// ===================================================================================
extern __code USB_DEV_DESCR DevDescr;
extern __code USB_CFG_DESCR_CDC CfgDescr;

// ===================================================================================
// String Descriptors
// ===================================================================================
extern __code uint16_t LangDescr[];
extern __code uint16_t SerDescr[];
extern __code uint16_t ProdDescr[];
extern __code uint16_t CDC_Descr[];
extern __code uint16_t ManufDescr[];

#define USB_STR_DESCR_i0    (uint8_t*)LangDescr
#define USB_STR_DESCR_i1    (uint8_t*)ManufDescr
#define USB_STR_DESCR_i2    (uint8_t*)ProdDescr
#define USB_STR_DESCR_i3    (uint8_t*)SerDescr
#define USB_STR_DESCR_i4    (uint8_t*)CDC_Descr
#define USB_STR_DESCR_ix    (uint8_t*)SerDescr
