// ===================================================================================
// USB HID Keyboard Functions for CH32X035/X034/X033                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// KBD_init()               init USB HID Keyboard
// KBD_press(k)             press a key on keyboard (see below for control keys)
// KBD_release(k)           release a key on keyboard
// KBD_type(k)              press and release a key on keyboard
// KBD_releaseAll()         release all keys on keyboard
// KBD_print(s)             type some text on the keyboard (string)
// KBD_getState();          get state of keyboard LEDs (see below)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "usb_hid.h"
#include "usb_handler.h"

// Functions
#define KBD_init   HID_init     // init keyboard
void KBD_press(uint8_t key);    // press a key on keyboard
void KBD_release(uint8_t key);  // release a key on keyboard
void KBD_type(uint8_t key);     // press and release a key on keyboard
void KBD_releaseAll(void);      // release all keys on keyboard
void KBD_print(char* str);      // type some text on the keyboard

// Keyboard LED states
#define KBD_getState()          (HID_status) 
#define KBD_NUM_LOCK_state      (KBD_getState() & 1)
#define KBD_CAPS_LOCK_state     ((KBD_getState() >> 1) & 1)
#define KBD_SCROLL_LOCK_state   ((KBD_getState() >> 2) & 1)
#define KBD_COMPOSE_state       ((KBD_getState() >> 3) & 1)
#define KBD_KANA_state          ((KBD_getState() >> 4) & 1)

// Modifier keys
#define KBD_KEY_LEFT_CTRL       0x80
#define KBD_KEY_LEFT_SHIFT      0x81
#define KBD_KEY_LEFT_ALT        0x82
#define KBD_KEY_LEFT_GUI        0x83
#define KBD_KEY_RIGHT_CTRL      0x84
#define KBD_KEY_RIGHT_SHIFT     0x85
#define KBD_KEY_RIGHT_ALT       0x86
#define KBD_KEY_RIGHT_GUI       0x87

// Special keys
#define KBD_KEY_UP_ARROW        0xDA
#define KBD_KEY_DOWN_ARROW      0xD9
#define KBD_KEY_LEFT_ARROW      0xD8
#define KBD_KEY_RIGHT_ARROW     0xD7
#define KBD_KEY_BACKSPACE       0xB2
#define KBD_KEY_TAB             0xB3
#define KBD_KEY_RETURN          0xB0
#define KBD_KEY_ESC             0xB1
#define KBD_KEY_INSERT          0xD1
#define KBD_KEY_DELETE          0xD4
#define KBD_KEY_PAGE_UP         0xD3
#define KBD_KEY_PAGE_DOWN       0xD6
#define KBD_KEY_HOME            0xD2
#define KBD_KEY_END             0xD5
#define KBD_KEY_CAPS_LOCK       0xC1
#define KBD_KEY_F1              0xC2
#define KBD_KEY_F2              0xC3
#define KBD_KEY_F3              0xC4
#define KBD_KEY_F4              0xC5
#define KBD_KEY_F5              0xC6
#define KBD_KEY_F6              0xC7
#define KBD_KEY_F7              0xC8
#define KBD_KEY_F8              0xC9
#define KBD_KEY_F9              0xCA
#define KBD_KEY_F10             0xCB
#define KBD_KEY_F11             0xCC
#define KBD_KEY_F12             0xCD
#define KBD_KEY_F13             0xF0
#define KBD_KEY_F14             0xF1
#define KBD_KEY_F15             0xF2
#define KBD_KEY_F16             0xF3
#define KBD_KEY_F17             0xF4
#define KBD_KEY_F18             0xF5
#define KBD_KEY_F19             0xF6
#define KBD_KEY_F20             0xF7
#define KBD_KEY_F21             0xF8
#define KBD_KEY_F22             0xF9
#define KBD_KEY_F23             0xFA
#define KBD_KEY_F24             0xFB

#ifdef __cplusplus
}
#endif
