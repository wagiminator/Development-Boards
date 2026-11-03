// ===================================================================================
// USB HID Transport for CMSIS-DAP v1 on CH32V003                             * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DAP_USB_init()           init DAP and USB
// DAP_USB_poll()           handle pending DAP requests (call in main loop)
//
// DAP requests are received as HID output reports via SET_REPORT control transfers
// on endpoint 0. The responses are sent as HID input reports via the interrupt IN
// endpoint 1 in 8-byte packets (low-speed USB). DAP_TransferAbort is executed
// immediately in the USB interrupt.
//
// References:
// -----------
// - CNLohr rv003usb: https://github.com/cnlohr/rv003usb
// - ARM CMSIS-DAP:   https://github.com/ARM-software/CMSIS-DAP
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "config.h"
#include "dap.h"
#include "usb_handler.h"

// Functions
void DAP_USB_init(void);                // init DAP and USB
void DAP_USB_poll(void);                // handle pending DAP requests
