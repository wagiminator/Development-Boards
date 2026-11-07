// ===================================================================================
// USB HID Host Functions for CH554                                           * v1.0 *
// ===================================================================================
//
// Simple functions for using USB HID devices (keyboard, mouse, gamepad, etc.)
// connected to the USB host port of the CH554, directly or via a USB hub. Based on
// the USB host stack in usb_host.c.
//
// Functions available:
// --------------------
// HOST_init()              init USB host
// HOST_update()            handle device connection/disconnection and enumeration,
//                          must be called frequently, returns 1 if devices changed
// HOST_printInfo()         print information on all connected devices (printf)
//
// HOST_findKeyboard()      search keyboard, returns 1 if found
// HOST_findMouse()         search mouse, returns 1 if found
// HOST_findHID()           search any HID device, returns 1 if found
//
// HOST_readKey()           read keyboard, returns ASCII character of a newly pressed
//                          key or 0 if there is none (US keyboard layout)
// HOST_readMouse()         read mouse, returns 1 if there is a new report, values
//                          are stored in HOST_mouse (buttons, x, y, wheel)
// HOST_readHID(ep)         read report from interrupt endpoint ep (0..3) of the HID
//                          device, returns report length (0: no new report), the
//                          report is stored in HOST_buffer
// HOST_setKbdLEDs(leds)    set keyboard LEDs (see below)
//
// Variables:
// ----------
// HOST_buffer[]            buffer with last received report
// HOST_mouse               last mouse report (buttons, x, y, wheel)
// HOST_kbdLEDs             current state of the keyboard lock keys/LEDs
// HOST_kbdModifiers        current state of the keyboard modifier keys
//
// Notes:
// ------
// - The USB host buffers are located at XRAM addresses 0x0000 - 0x007F, therefore
//   XRAM_LOC in the makefile must be at least 0x0080.
// - Keyboards and mice are used in boot protocol mode.
// - F_CPU should be 24MHz (requires 5V supply).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "usb_host.h"

// Keyboard LED bits
#define HOST_LED_NUM      0x01
#define HOST_LED_CAPS     0x02
#define HOST_LED_SCROLL   0x04

// Mouse report
typedef struct {
  uint8_t buttons;                  // bit 0: left, bit 1: right, bit 2: middle
  int8_t  x;                        // relative movement in x-direction
  int8_t  y;                        // relative movement in y-direction
  int8_t  wheel;                    // relative movement of the wheel
} HOST_MOUSE_REPORT;

extern __xdata uint8_t HOST_buffer[];
extern __xdata HOST_MOUSE_REPORT HOST_mouse;
extern uint8_t HOST_kbdLEDs;
extern uint8_t HOST_kbdModifiers;

// Functions
void    HOST_init(void);
uint8_t HOST_update(void);
void    HOST_printInfo(void);
uint8_t HOST_findKeyboard(void);
uint8_t HOST_findMouse(void);
uint8_t HOST_findHID(void);
char    HOST_readKey(void);
uint8_t HOST_readMouse(void);
uint8_t HOST_readHID(uint8_t ep);
void    HOST_setKbdLEDs(uint8_t leds);
