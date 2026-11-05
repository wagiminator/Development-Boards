// ===================================================================================
// USB Handler for CH32X035/X034/X033                                         * v1.3 *
// ===================================================================================
//
// Endpoint functions (used by the class drivers):
// -----------------------------------------------
// USB_EP_init(ep,type,buf,out,in) setup endpoint ep (1..3, 5..7) with buffer buf
// USB_EP_send(ep, len)     transmit len bytes from IN buffer of endpoint ep
// USB_EP_receive(ep)       ready to receive next OUT packet on endpoint ep (ACK)
// USB_EP_hold(ep)          do not accept OUT packets on endpoint ep (NAK)
// USB_EP_abort(ep)         cancel pending IN transfer of endpoint ep (NAK)
// USB_RX_len               number of bytes of the last received OUT packet
//
// After an OUT packet has been received, the endpoint responds with NAK until
// USB_EP_receive() is called. After an IN packet has been sent, the endpoint
// responds with NAK until USB_EP_send() is called again.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "usb_descr.h"

// ===================================================================================
// USB Handler Parameters and Checks
// ===================================================================================
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// MCU supply voltage (0: 3.3V, 1: 5V) can be defined as USB_VDD in config.h.
// If USB_VDD is not defined, the supply voltage is detected via the PVD on init.

// ===================================================================================
// Custom External USB Handler Functions (usb_dap.c, usb_cdc.c)
// ===================================================================================
#include "usb_dap.h"
#include "usb_cdc.h"

// ===================================================================================
// USB Handler Defines
// ===================================================================================
// Custom USB handler functions
#define USB_INIT_endpoints        DAP_EP_init       // custom USB EP init handler
#if USB_WINUSB > 0
#define USB_VENDOR_SETUP_handler  DAP_vendorSetup   // MS OS 2.0 descriptor request
#endif
#if USB_CDC_ENABLE > 0
#define USB_CLASS_SETUP_handler   CDC_control       // CDC class requests
#define USB_CLASS_OUT_handler     CDC_EP0_OUT       // CDC class OUT data stage
#endif

// Endpoint callback functions
#define EP1_IN_callback           DAP_EP1_IN        // DAP response
#define EP1_OUT_callback          DAP_EP1_OUT       // DAP request
#define EP2_IN_callback           DAP_EP2_IN        // SWO trace
#if USB_CDC_ENABLE > 0
#define EP5_IN_callback           CDC_EP5_IN        // CDC data IN
#define EP5_OUT_callback          CDC_EP5_OUT       // CDC data OUT
#endif

// ===================================================================================
// Variables
// ===================================================================================
#define USB_SetupBuf     ((PUSB_SETUP_REQ)EP0_buffer)
extern volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
extern volatile uint16_t USB_SetupLen;
extern volatile uint8_t  USB_RX_len;
extern const uint8_t*    USB_pDescr;

// ===================================================================================
// Functions
// ===================================================================================
void USB_init(void);
void USB_EP0_copyDescr(uint8_t len);
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in);
void USB_EP_send(uint8_t ep, uint8_t len);
void USB_EP_receive(uint8_t ep);
void USB_EP_hold(uint8_t ep);
void USB_EP_abort(uint8_t ep);

#ifdef __cplusplus
}
#endif
