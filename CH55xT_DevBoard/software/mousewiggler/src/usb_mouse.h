// ===================================================================================
// USB HID Standard Mouse Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "usb_hid.h"

// Functions
#define MOUSE_init() HID_init()             // init mouse
void MOUSE_press(uint8_t buttons);          // press button(s)
void MOUSE_release(uint8_t buttons);        // release button(s)
void MOUSE_move(int8_t xrel, int8_t yrel);  // move mouse pointer (relative)
void MOUSE_wheel(int8_t rel);               // move mouse wheel (relative)

// Mouse buttons
#define MOUSE_BUTTON_LEFT     0x01          // left mouse button
#define MOUSE_BUTTON_RIGHT    0x02          // right mouse button
#define MOUSE_BUTTON_MIDDLE   0x04          // middle mouse button
