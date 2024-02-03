// ===================================================================================
// USB HID Functions for CH32X035/X034/X033                                   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// HID_init()               init USB-HID
// HID_sendReport(rep, len) send HID report (pointer to report buffer, length)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "system.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"

#define HID_IN_buffer   EP2_buffer                // buffer for incoming HID reports
#define HID_init        USB_init                  // setup USB-HID
void HID_sendReport(uint8_t* buf, uint8_t len);   // send HID report

#ifdef EP2_SIZE
extern volatile uint8_t HID_status;               // status byte returned from host
#endif

#ifdef __cplusplus
}
#endif
