// ===================================================================================
// USB HID Functions for STC8H8K64U                                           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// HID_init()               init USB-HID
// HID_ready()              check if HID is ready to send a report
// HID_sendReport(rep, len) send HID report (pointer to report buffer, length)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"

#ifdef EP2_SIZE
#define HID_IN_buffer   EP2_buffer                        // buffer for incoming HID reports
#endif

extern volatile __bit HID_writeBusyFlag;                  // upload busy flag
extern volatile uint8_t HID_idleRate, HID_protocol;       // HID idle rate and protocol

#define HID_init        USB_init                          // setup USB-HID
#define HID_ready()     (!HID_writeBusyFlag)              // ready to send report?
void HID_sendReport(__xdata uint8_t* buf, uint8_t len);   // send HID report
