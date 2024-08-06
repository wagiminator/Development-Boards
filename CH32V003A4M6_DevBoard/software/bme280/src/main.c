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
// Reads temperature, pressure and humidity via I2C BME280 sensor (SDA: PC1, SCL: PC2) 
// and sends data via serial interface (UART BAUD: 115200, 8N1, TX: PD5).
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
#include <bme280.h>                 // BME280 sensor functions
#include <debug_serial.h>           // serial debug functions


// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                     // init serial interface
  BME_init();                       // init I2C BME280 sensor

  // Loop
  while(1) {
    int32_t  temp  = BME_getTemp();
    uint32_t press = BME_getPressure();
    uint32_t humid = BME_getHumidity();
    DEBUG_printf("Temperature: %d.%02d DegC\n", temp / 100, temp % 100);
    DEBUG_printf("Pressure:    %d.%02d hPa\n", press / 100, press % 100);
    DEBUG_printf("Humidity:    %d.%02d %%RH\n", humid / 100, humid % 100);
    DLY_ms(1000);
  }
}
