// ===================================================================================
// TOYOSHIKI Tiny BASIC
// ===================================================================================
// (C) 2012 Tetsuya Suzuki, GNU General Public License
// https://github.com/rutles/ttbasic_arduino

#include "uart_dma.h"

// Define special keys
#define KEY_ENTER     10      // ENTER key: LF
#define KEY_ABORT     27      // ABORT key: ESC

// TOYOSHIKI Tiny BASIC symbols
// TO-DO Rewrite defined values to fit your machine as needed
#define SIZE_LINE     80      // Command line buffer length + NULL
#define SIZE_IBUF     80      // i-code conversion buffer size
#define SIZE_LIST     1024    // List buffer size
#define SIZE_ARRY     32      // Array area size
#define SIZE_GSTK     6       // GOSUB stack size (2/nest)
#define SIZE_LSTK     15      // FOR stack size (5/nest)

// Edition string
#define STR_EDITION   "CH32V003"

// Terminal control
#define c_putch(c)    UART_write(c)
#define c_getch( )    UART_read()
#define c_kbhit( )    UART_available()

// Functions
void basic();
