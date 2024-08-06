// ===================================================================================
// Project:   TOYOSHIKI Tiny BASIC for CH32V003
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// A simple BASIC interpreter based on TOYOSHIKI Tiny BASIC using a UART terminal.
//
// References:
// -----------
// - Tetsuya Suzuki, TOYOSHIKI Tiny BASIC: https://github.com/rutles/ttbasic_arduino
// - CNLohr, ch32v003fun:                  https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with rvprog
//   are installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the device via USB-to-serial converter to your PC.
// - Open a serial terminal and select the proper COM port.
// - Write and execute your BASIC program.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <ttbasic.h>                // TOYOSHIKI Tiny BASIC

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  UART_init();                      // init UART (TX: PD5, RX: PD6, BAUD: 115200, 8N1)
  while(1) basic();                 // run BASIC
}
