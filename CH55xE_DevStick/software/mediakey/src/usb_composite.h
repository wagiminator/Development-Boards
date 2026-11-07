// ===================================================================================
// USB HID Composite Device Functions for CH551, CH552 and CH554              * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// HID_init()               init HID composite device
//
// KBD_press(k)             press a key on keyboard (see below for control keys)
// KBD_release(k)           release a key on keyboard
// KBD_type(k)              press and release a key on keyboard
// KBD_releaseAll()         release all keys on keyboard
// KBD_print(s)             type some text on the keyboard (string)
// KBD_getState();          get state of keyboard LEDs (see below)
//
// CON_press(k)             press a consumer/multimedia key (see below)
// CON_release()            release consumer/multimedia key
// CON_type(k)              press and release a consumer/multimedia key
// 
// MOUSE_press(b)           press button(s) (see below)
// MOUSE_release(b)         release button(s)
// MOUSE_move(x,y)          move mouse pointer (relative)
// MOUSE_wheel(w)           move mouse wheel (relative)
// MOUSE_wheel_up()         move mouse wheel one step up
// MOUSE_wheel_down()       move mouse wheel one step down
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "usb_hid.h"
#include "usb_handler.h"

// Functions
void KBD_press(uint8_t key);                // press a key on keyboard
void KBD_release(uint8_t key);              // release a key on keyboard
void KBD_type(uint8_t key);                 // press and release a key on keyboard
void KBD_releaseAll(void);                  // release all keys on keyboard
void KBD_print(char* str);                  // type some text on the keyboard

void CON_press(uint8_t key);                // press a consumer key on keyboard
void CON_release(void);                     // release consumer key on keyboard
void CON_type(uint8_t key);                 // press and release a consumer key

void MOUSE_press(uint8_t buttons);          // press mouse button(s)
void MOUSE_release(uint8_t buttons);        // release mouse button(s)
void MOUSE_move(int8_t xrel, int8_t yrel);  // move mouse pointer (relative)
void MOUSE_wheel(int8_t rel);               // move mouse wheel (relative)

#define MOUSE_wheel_up()        MOUSE_wheel( 1)
#define MOUSE_wheel_down()      MOUSE_wheel(-1)

// Keyboard LED states
#define KBD_getState()          (HID_IN_buffer[0]) 
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

// Consumer Keyboard Keycodes
#define CON_SYS_POWER           0x30
#define CON_SYS_RESET           0x31
#define CON_SYS_SLEEP           0x32

#define CON_VOL_MUTE            0xE2
#define CON_VOL_UP              0xE9
#define CON_VOL_DOWN            0xEA

#define CON_MEDIA_PLAY          0xB0
#define CON_MEDIA_PAUSE         0xB1
#define CON_MEDIA_RECORD        0xB2
#define CON_MEDIA_FORWARD       0xB3
#define CON_MEDIA_REWIND        0xB4
#define CON_MEDIA_NEXT          0xB5
#define CON_MEDIA_PREV          0xB6
#define CON_MEDIA_STOP          0xB7
#define CON_MEDIA_EJECT         0xB8
#define CON_MEDIA_RANDOM        0xB9

#define CON_MENU                0x40
#define CON_MENU_PICK           0x41
#define CON_MENU_UP             0x42
#define CON_MENU_DOWN           0x43
#define CON_MENU_LEFT           0x44
#define CON_MENU_RIGHT          0x45
#define CON_MENU_ESCAPE         0x46
#define CON_MENU_INCR           0x47
#define CON_MENU_DECR           0x48

// Mouse Buttons
#define MOUSE_BUTTON_LEFT       0x01
#define MOUSE_BUTTON_RIGHT      0x02
#define MOUSE_BUTTON_MIDDLE     0x04
