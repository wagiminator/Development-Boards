// ===================================================================================
// USB HID Standard Mouse Functions for CH32V003                              * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// MOUSE_init()             init USB HID Mouse
// MOUSE_press(b)           press button(s) (see below)
// MOUSE_release(b)         release button(s)
// MOUSE_move(x,y)          move mouse pointer (relative)
// MOUSE_wheel(w)           move mouse wheel (relative)
// MOUSE_wheel_up()         move mouse wheel one step up
// MOUSE_wheel_down()       move mouse wheel one step down
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "usb_handler.h"

// Functions
#define MOUSE_init usb_setup                // init mouse
void MOUSE_press(uint8_t buttons);          // press button(s)
void MOUSE_release(uint8_t buttons);        // release button(s)
void MOUSE_move(int8_t xrel, int8_t yrel);  // move mouse pointer (relative)
void MOUSE_wheel(int8_t rel);               // move mouse wheel (relative)

#define MOUSE_wheel_up()    MOUSE_wheel( 1) // move mouse wheel one step up
#define MOUSE_wheel_down()  MOUSE_wheel(-1) // move mouse wheel one step down

// Mouse buttons
#define MOUSE_BUTTON_LEFT     0x01          // left mouse button
#define MOUSE_BUTTON_RIGHT    0x02          // right mouse button
#define MOUSE_BUTTON_MIDDLE   0x04          // middle mouse button

#ifdef __cplusplus
}
#endif
