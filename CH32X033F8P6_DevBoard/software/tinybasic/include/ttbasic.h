// ===================================================================================
// TOYOSHIKI Tiny BASIC
// ===================================================================================
// (C) 2012 Tetsuya Suzuki, GNU General Public License
// https://github.com/rutles/ttbasic_arduino

#include "usb_cdc.h"

// Define special keys
#define KEY_ENTER     13      // ENTER key: CR
#define KEY_ABORT     27      // ABORT key: ESC

// TOYOSHIKI Tiny BASIC symbols
// TO-DO Rewrite defined values to fit your machine as needed
#define SIZE_LINE     80      // Command line buffer length + NULL
#define SIZE_IBUF     80      // i-code conversion buffer size
#define SIZE_LIST     16384   // List buffer size
#define SIZE_ARRY     32      // Array area size
#define SIZE_GSTK     6       // GOSUB stack size (2/nest)
#define SIZE_LSTK     15      // FOR stack size (5/nest)

// Edition string
#define STR_EDITION   "CH32X035"

// Terminal control
#define c_putch(c)    CDC_writeflush(c)
#define c_getch( )    CDC_read()
#define c_kbhit( )    CDC_available()

// Functions
void basic();
