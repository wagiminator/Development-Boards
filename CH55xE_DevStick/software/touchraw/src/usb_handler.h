// ===================================================================================
// USB Handler for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "usb_descr.h"

// ===================================================================================
// Endpoint Buffer
// ===================================================================================
__xdata __at (EP0_ADDR) uint8_t EP0_buffer[EP0_BUF_SIZE];     
__xdata __at (EP1_ADDR) uint8_t EP1_buffer[EP1_BUF_SIZE];
__xdata __at (EP2_ADDR) uint8_t EP2_buffer[2 * EP2_BUF_SIZE];

#define USB_setupBuf ((PUSB_SETUP_REQ)EP0_buffer)
extern uint8_t SetupReq;

// ===================================================================================
// Custom External USB Handler Functions
// ===================================================================================
uint8_t CDC_control(void);
void CDC_setup(void);
void CDC_reset(void);
void CDC_EP0_OUT(void);
void CDC_EP1_IN(void);
void CDC_EP2_IN(void);
void CDC_EP2_OUT(void);

// ===================================================================================
// USB Handler Defines
// ===================================================================================
// Custom USB handler functions
#define USB_INIT_handler    CDC_setup         // init custom endpoints
#define USB_RESET_handler   CDC_reset         // custom USB reset handler
#define USB_CTRL_NS_handler CDC_control       // handle custom non-standard requests

// Endpoint callback functions
#define EP0_SETUP_callback  USB_EP0_SETUP
#define EP0_IN_callback     USB_EP0_IN
#define EP0_OUT_callback    CDC_EP0_OUT
#define EP1_IN_callback     CDC_EP1_IN
#define EP2_IN_callback     CDC_EP2_IN
#define EP2_OUT_callback    CDC_EP2_OUT

// ===================================================================================
// Functions
// ===================================================================================
void USB_interrupt(void);
void USB_init(void);
