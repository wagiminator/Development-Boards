// ===================================================================================
// Project:   Example for CH32V003
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
// Displays supply voltage on a 4-digit 8-segment LED Display via TM1650 and I2C.
// Connections: pin PC1 (SDA) and PC2 (SCL).
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with rvprog
//   are installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>                               // system functions
#include <tm1650.h>                               // TM1650 functions
#include <gpio.h>                                 // GPIO/ADC functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  ADC_init();                                     // init ADC
  ADC_slow();                                     // slow but more accurate conversion
  TM_init();                                      // init TM1650 display

  // Loop
  while(1) {
    TM_print(ADC_read_VDD(), 3);                  // print supply voltage on display
    DLY_ms(500);                                  // wait a bit
  }
}
