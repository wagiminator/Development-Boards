// ===================================================================================
// ISP Functions for CH32V003                                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "isp.h"

// Global variables
uint8_t ISP_sck = USBASP_ISP_SCK_AUTO;    // selected SCK option
static uint8_t  ISP_hiaddr;               // current extended address byte
static uint8_t  ISP_hwspi;                // 1: use hardware SPI, 0: use software SPI
static uint32_t ISP_swdelay;              // software SPI: half clock period in ticks

// Software SPI frequencies for SCK options 1..8
static const uint32_t ISP_swfreq[] = {
  500, 1000, 2000, 4000, 8000, 16000, 32000, 93750
};

// ===================================================================================
// SPI Low-Level Functions
// ===================================================================================

// Set SCK option (hardware SPI is disabled, software SPI delay is set)
static void ISP_setSCK(uint8_t option) {
  SPI1->CTLR1 = 0;                                  // disable hardware SPI
  if(option == USBASP_ISP_SCK_AUTO) option = USBASP_ISP_SCK_1500;
  if(option > USBASP_ISP_SCK_3000)  option = USBASP_ISP_SCK_3000;
  if(option >= USBASP_ISP_SCK_187_5) {
    // Hardware SPI: SCK = F_CPU / (2 << BR), BR = 3 (3MHz) .. 7 (187.5kHz)
    ISP_hwspi   = 1;
    SPI1->CTLR1 = ((16 - option) << 3)              // set prescaler
                | SPI_CTLR1_MSTR                    // master configuration, mode 0
                | SPI_CTLR1_SSM                     // software control of NSS
                | SPI_CTLR1_SSI;                    // set internal NSS high
  }
  else {
    // Software SPI for lower frequencies
    ISP_hwspi   = 0;
    ISP_swdelay = F_CPU / 2 / ISP_swfreq[option - 1];
  }
}

// Enable SPI pins (SCK and MOSI are driven low or by the SPI module)
static void ISP_SPI_enable(void) {
  PIN_low(PIN_SCK);                                 // SCK low
  PIN_low(PIN_MOSI);                                // MOSI low
  PIN_input_PU(PIN_MISO);                           // MISO input with pullup
  if(ISP_hwspi) {
    PIN_alternate(PIN_SCK);                         // SCK to SPI
    PIN_alternate(PIN_MOSI);                        // MOSI to SPI
    SPI1->CTLR1 |= SPI_CTLR1_SPE;                   // enable SPI module
    (void)SPI1->DATAR;                              // clear receive buffer
  }
  else {
    PIN_output(PIN_SCK);                            // SCK to output
    PIN_output(PIN_MOSI);                           // MOSI to output
  }
}

// Disable hardware SPI module (SCK and MOSI are driven low)
static void ISP_SPI_disable(void) {
  PIN_low(PIN_SCK);
  PIN_low(PIN_MOSI);
  PIN_output(PIN_SCK);
  PIN_output(PIN_MOSI);
  SPI1->CTLR1 = 0;
}

// Transmit and receive one byte via software SPI (mode 0, MSB first)
static uint8_t ISP_SW_transmit(uint8_t data) {
  uint8_t i;
  for(i=8; i; i--) {
    PIN_write(PIN_MOSI, data & 0x80);               // set MOSI according to MSB
    data <<= 1;                                     // shift to next bit
    DLY_ticks(ISP_swdelay);                         // wait half a clock period
    PIN_high(PIN_SCK);                              // SCK high
    data |= PIN_read(PIN_MISO);                     // sample MISO
    DLY_ticks(ISP_swdelay);                         // wait half a clock period
    PIN_low(PIN_SCK);                               // SCK low
  }
  return data;
}

// Transmit and receive one byte via ISP
uint8_t ISP_transmit(uint8_t data) {
  if(ISP_hwspi) {
    SPI1->DATAR = data;                             // send data byte
    while(!(SPI1->STATR & SPI_STATR_RXNE));         // wait for reply
    return SPI1->DATAR;                             // return received data byte
  }
  return ISP_SW_transmit(data);
}

// ===================================================================================
// ISP Bus Functions
// ===================================================================================

// Connect ISP bus
void ISP_connect(void) {
  RCC->APB2PCENR |= RCC_SPI1EN | RCC_AFIOEN;        // enable SPI module clock
  ISP_LED_on();                                     // turn on status LED
  ISP_setSCK(ISP_sck);                              // set SCK option
  ISP_SPI_enable();                                 // set SPI pins
  PIN_high(PIN_RESET);                              // positive pulse on RESET
  PIN_output(PIN_RESET);
  DLY_us(320);
  PIN_low(PIN_RESET);
  ISP_hiaddr = 0xFF;                                // force update of extended address
}

// Disconnect ISP bus (all pins to high impedance)
void ISP_disconnect(void) {
  SPI1->CTLR1 = 0;                                  // disable SPI module
  PIN_input(PIN_RESET);
  PIN_input(PIN_SCK);
  PIN_input(PIN_MOSI);
  PIN_input(PIN_MISO);
  ISP_sck = USBASP_ISP_SCK_AUTO;                    // reset SCK option to auto
  ISP_LED_off();                                    // turn off status LED
}

// Enter programming mode with auto-clock (thanks to Ralph Doncaster):
// If the target doesn't answer, the SCK frequency is reduced step by step.
uint8_t ISP_enterProgrammingMode(void) {
  uint8_t check, tries;

  if(ISP_sck == USBASP_ISP_SCK_AUTO) ISP_sck = USBASP_ISP_SCK_1500;

  while(ISP_sck >= USBASP_ISP_SCK_0_5) {
    ISP_setSCK(ISP_sck);                            // set SCK option
    ISP_SPI_enable();                               // set SPI pins
    tries = 3;
    do {
      PIN_high(PIN_RESET);                          // pulse RESET
      DLY_us(320);
      PIN_low(PIN_RESET);
      DLY_ms(20);                                   // wait before programming enable

      ISP_transmit(0xAC);                           // programming enable command
      ISP_transmit(0x53);
      check = ISP_transmit(0);
      ISP_transmit(0);

      if(check == 0x53) return 0;                   // correct answer? -> success
    } while(--tries);

    ISP_SPI_disable();                              // disable hardware SPI
    ISP_sck--;                                      // try lower speed
  }
  return 1;                                         // error: device doesn't answer
}

// ===================================================================================
// ISP Memory Functions
// ===================================================================================

// Update extended address byte
static void ISP_updateExtended(uint32_t address) {
  uint8_t curr_hiaddr = address >> 17;
  if(ISP_hiaddr != curr_hiaddr) {                   // extended address byte changed?
    ISP_hiaddr = curr_hiaddr;
    ISP_transmit(0x4D);                             // load extended address byte
    ISP_transmit(0x00);
    ISP_transmit(ISP_hiaddr);
    ISP_transmit(0x00);
  }
}

// Poll flash until the byte at address is no longer equal to value (max 9.6ms)
static uint8_t ISP_pollFlash(uint32_t address, uint8_t value) {
  uint32_t start = STK->CNT;
  while((STK->CNT - start) < (9600 * DLY_US_TIME)) {
    if(ISP_readFlash(address) != value) return 0;
  }
  return 1;                                         // error: timeout
}

// Read flash byte
uint8_t ISP_readFlash(uint32_t address) {
  ISP_updateExtended(address);
  ISP_transmit(0x20 | ((address & 1) << 3));
  ISP_transmit(address >> 9);
  ISP_transmit(address >> 1);
  return ISP_transmit(0);
}

// Write flash byte (pollmode = 0: load page buffer only)
uint8_t ISP_writeFlash(uint32_t address, uint8_t data, uint8_t pollmode) {
  ISP_updateExtended(address);
  ISP_transmit(0x40 | ((address & 1) << 3));
  ISP_transmit(address >> 9);
  ISP_transmit(address >> 1);
  ISP_transmit(data);

  if(!pollmode) return 0;
  if(data == 0x7F) {
    DLY_us(4800);                                   // polling not possible, just wait
    return 0;
  }
  return ISP_pollFlash(address, 0x7F);
}

// Write flash page
uint8_t ISP_flushPage(uint32_t address, uint8_t pollvalue) {
  ISP_updateExtended(address);
  ISP_transmit(0x4C);                               // write program memory page
  ISP_transmit(address >> 9);
  ISP_transmit(address >> 1);
  ISP_transmit(0);

  if(pollvalue == 0xFF) {
    DLY_us(4800);                                   // polling not possible, just wait
    return 0;
  }
  return ISP_pollFlash(address, 0xFF);
}

// Read EEPROM byte
uint8_t ISP_readEEPROM(uint16_t address) {
  ISP_transmit(0xA0);
  ISP_transmit(address >> 8);
  ISP_transmit(address);
  return ISP_transmit(0);
}

// Write EEPROM byte
uint8_t ISP_writeEEPROM(uint16_t address, uint8_t data) {
  ISP_transmit(0xC0);
  ISP_transmit(address >> 8);
  ISP_transmit(address);
  ISP_transmit(data);
  DLY_us(9600);                                     // wait 9.6ms
  return 0;
}
