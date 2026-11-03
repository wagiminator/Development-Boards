// ===================================================================================
// USBasp Functions for CH32V003                                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// ASP_init()               init ISP/TPI bus, status LEDs and USB
// ASP_poll()               handle pending USBasp requests (call in main loop)
// ASP_USB_connected()      check if device is enumerated by the host
//
// All USBasp requests are vendor-specific control transfers on endpoint 0. They are
// received by the USB interrupt and executed by ASP_poll() in the main loop. In the
// meantime, the USB interrupt answers the host with NAK.
//
// References:
// -----------
// - Thomas Fischl:   https://www.fischl.de/usbasp/
// - Ralph Doncaster: https://github.com/nerdralph/usbasp
// - CNLohr rv003usb: https://github.com/cnlohr/rv003usb
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "config.h"
#include "isp.h"
#include "tpi.h"
#include "usb_handler.h"

// USB function call identifiers
#define USBASP_FUNC_CONNECT           1
#define USBASP_FUNC_DISCONNECT        2
#define USBASP_FUNC_TRANSMIT          3
#define USBASP_FUNC_READFLASH         4
#define USBASP_FUNC_ENABLEPROG        5
#define USBASP_FUNC_WRITEFLASH        6
#define USBASP_FUNC_READEEPROM        7
#define USBASP_FUNC_WRITEEEPROM       8
#define USBASP_FUNC_SETLONGADDRESS    9
#define USBASP_FUNC_SETISPSCK         10
#define USBASP_FUNC_TPI_CONNECT       11
#define USBASP_FUNC_TPI_DISCONNECT    12
#define USBASP_FUNC_TPI_RAWREAD       13
#define USBASP_FUNC_TPI_RAWWRITE      14
#define USBASP_FUNC_TPI_READBLOCK     15
#define USBASP_FUNC_TPI_WRITEBLOCK    16
#define USBASP_FUNC_GETCAPABILITIES   127

// USBASP capabilities
#define USBASP_CAP_0_TPI              0x01    // byte 0: TPI support
#define USBASP_CAP_3_3MHZ             0x01    // byte 3: 3MHz SCK support

// Block mode flags
#define PROG_BLOCKFLAG_FIRST          1
#define PROG_BLOCKFLAG_LAST           2

// Size of the data buffer (AVRDUDE uses blocks of up to 200 bytes)
#define ASP_BUF_SIZE                  256

// Functions
void ASP_init(void);                    // init ISP/TPI bus, LEDs and USB
void ASP_poll(void);                    // handle pending USBasp requests
#define ASP_USB_connected()           (rv003usb_internal_data.my_address != 0)

// USB status LED
#ifdef PIN_LED_USB
  #define ASP_LED_USB_init()          {PIN_high(PIN_LED_USB); PIN_output(PIN_LED_USB);}
  #define ASP_LED_USB_write(on)       PIN_write(PIN_LED_USB, !(on))
#else
  #define ASP_LED_USB_init()
  #define ASP_LED_USB_write(on)
#endif
