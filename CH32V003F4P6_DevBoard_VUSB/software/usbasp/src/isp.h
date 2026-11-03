// ===================================================================================
// ISP Functions for CH32V003                                                 * v1.0 *
// ===================================================================================
//
// AVR in-system programming via SPI. The hardware SPI of the CH32V003 is used for
// SCK frequencies from 187.5kHz to 3MHz, software SPI for lower frequencies.
// The automatic clock selection (auto-clock) is based on the implementation by
// Ralph Doncaster (https://github.com/nerdralph/usbasp).
//
// SPI pin mapping (hardware SPI, cannot be changed):
// SCK-pin   PC5
// MOSI-pin  PC6
// MISO-pin  PC7
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "system.h"
#include "gpio.h"
#include "config.h"

// ISP SCK speed identifiers
#define USBASP_ISP_SCK_AUTO       0
#define USBASP_ISP_SCK_0_5        1     // 500 Hz
#define USBASP_ISP_SCK_1          2     //   1 kHz
#define USBASP_ISP_SCK_2          3     //   2 kHz
#define USBASP_ISP_SCK_4          4     //   4 kHz
#define USBASP_ISP_SCK_8          5     //   8 kHz
#define USBASP_ISP_SCK_16         6     //  16 kHz
#define USBASP_ISP_SCK_32         7     //  32 kHz
#define USBASP_ISP_SCK_93_75      8     //  93.75 kHz
#define USBASP_ISP_SCK_187_5      9     // 187.5  kHz
#define USBASP_ISP_SCK_375        10    // 375 kHz
#define USBASP_ISP_SCK_750        11    // 750 kHz
#define USBASP_ISP_SCK_1500       12    //   1.5 MHz
#define USBASP_ISP_SCK_3000       13    //   3 MHz

// Selected SCK option (set by host, used by ISP_enterProgrammingMode)
extern uint8_t ISP_sck;

// Functions
void ISP_connect(void);                                                   // connect ISP bus
void ISP_disconnect(void);                                                // disconnect ISP bus
uint8_t ISP_transmit(uint8_t data);                                       // transmit/receive byte
uint8_t ISP_enterProgrammingMode(void);                                   // enter programming mode
uint8_t ISP_readFlash(uint32_t address);                                  // read flash byte
uint8_t ISP_writeFlash(uint32_t address, uint8_t data, uint8_t pollmode); // write flash byte
uint8_t ISP_flushPage(uint32_t address, uint8_t pollvalue);               // write flash page
uint8_t ISP_readEEPROM(uint16_t address);                                 // read EEPROM byte
uint8_t ISP_writeEEPROM(uint16_t address, uint8_t data);                  // write EEPROM byte

// Status LED
#ifdef PIN_LED_PRG
  #define ISP_LED_init()      {PIN_high(PIN_LED_PRG); PIN_output(PIN_LED_PRG);}
  #define ISP_LED_on()        PIN_low(PIN_LED_PRG)
  #define ISP_LED_off()       PIN_high(PIN_LED_PRG)
#else
  #define ISP_LED_init()
  #define ISP_LED_on()
  #define ISP_LED_off()
#endif
