// ===================================================================================
// USB Handler for STC8H8K64U                                                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// USB_init()               init USB peripheral and endpoints, enable interrupts
// USB_interrupt()          USB handler, must be called by the USB interrupt service
//                          routine in main.c (see notes)
//
// Endpoint functions (used by the class drivers):
// -----------------------------------------------
// USB_EP_init(ep,buf,out,in) setup endpoint ep (1..5) with buffer buf
//                          (out=1: OUT enabled, in=1: IN enabled, both: IN at buf+64)
// USB_EP_send(ep, len)     transmit len bytes from IN buffer of endpoint ep
// USB_EP_receive(ep)       ready to receive next OUT packet on endpoint ep (ACK)
// USB_RX_len               number of bytes of the last received OUT packet
//
// After an OUT packet has been received, the endpoint responds with NAK until
// USB_EP_receive() is called. The EPx_IN_callback is called after the packet
// transmitted with USB_EP_send() has been accepted by the host.
//
// Notes:
// ------
// The USB interrupt service routine must be defined in main.c (SDCC requirement):
// void USB_ISR(void) __interrupt(INT_NO_USB) { USB_interrupt(); }
// The USB peripheral requires the internal 48MHz oscillator (IRC48M), which is
// enabled in USB_init(). The system clock (F_CPU) can be chosen independently.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"
#include "usb.h"
#include "usb_descr.h"

// ===================================================================================
// Variables
// ===================================================================================
#define USB_SetupBuf     ((PXUSB_SETUP_REQ)EP0_buffer)
extern volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_RX_len;
extern volatile uint16_t USB_SetupLen;
extern volatile __bit    USB_ENUM_OK;
extern __code uint8_t*   USB_pDescr;

// ===================================================================================
// Functions
// ===================================================================================
void USB_init(void);
void USB_interrupt(void);
void USB_EP0_copyDescr(uint8_t len);
void USB_EP_init(uint8_t ep, __xdata uint8_t* buf, uint8_t out, uint8_t in);
void USB_EP_send(uint8_t ep, uint8_t len);
void USB_EP_receive(uint8_t ep);
