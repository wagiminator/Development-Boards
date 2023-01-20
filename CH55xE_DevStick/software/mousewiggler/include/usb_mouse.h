// ===================================================================================
// USB HID Standard Mouse Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "usb_hid.h"

// Functions
#define MOUSE_init() HID_init()             // init mouse
void MOUSE_move(int8_t xrel, int8_t yrel);  // move mouse (relative)
void MOUSE_press(uint8_t buttons);          // press button(s)
void MOUSE_release(uint8_t buttons);        // release button(s)

// Mouse buttons
#define MOUSE_BUTTON_RIGHT    0x01          // right mouse button
#define MOUSE_BUTTON_MIDDLE   0x02          // middle mouse button
#define MOUSE_BUTTON_LEFT     0x04          // left mouse button
