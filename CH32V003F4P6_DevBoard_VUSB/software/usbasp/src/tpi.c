// ===================================================================================
// TPI Functions for CH32V003                                                 * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "tpi.h"
#include "isp.h"

// Global variables
uint16_t TPI_dly_cnt;                     // TPI clock delay (set by host)
static uint32_t TPI_delay;                // half TPI clock period in system ticks

// ===================================================================================
// TPI Low-Level Functions
// ===================================================================================

// Transmit/receive one bit via TPI; b must be 1 when receiving
static uint8_t TPI_clockBit(uint8_t b) {
  if(b) PIN_input_PU(PIN_MOSI);           // DATA high: release line (pullup)
  else {
    PIN_low(PIN_MOSI);                    // DATA low: drive line low
    PIN_output(PIN_MOSI);
  }
  DLY_ticks(TPI_delay);                   // wait half a clock period
  PIN_high(PIN_SCK);                      // CLK high
  b = PIN_read(PIN_MOSI);                 // read DATA
  DLY_ticks(TPI_delay);                   // wait half a clock period
  PIN_low(PIN_SCK);                       // CLK low
  return b;
}

// Send break (2 breaks, 24++ bits low) and idle bit
static void TPI_break(void) {
  uint8_t i;
  for(i=26; i; i--) TPI_clockBit(0);
  TPI_clockBit(1);
}

// Get even parity of a byte
static uint8_t TPI_parity(uint8_t b) {
  b ^= b >> 4;
  b ^= b >> 2;
  b ^= b >> 1;
  return b & 1;
}

// ===================================================================================
// TPI Bus Functions
// ===================================================================================

// Connect TPI bus
void TPI_connect(void) {
  uint8_t i;
  ISP_LED_on();                           // turn on status LED
  // TPI clock: avrdude sets 1 count = 1/3us (12MHz AVR loop), 16 ticks @ 48MHz
  TPI_delay = (uint32_t)TPI_dly_cnt * (F_CPU / 3000000) + (F_CPU / 2000000);
  PIN_high(PIN_RESET);                    // RESET high
  PIN_output(PIN_RESET);                  // RESET to output
  DLY_us(1000);                           // wait a bit
  PIN_low(PIN_RESET);                     // RESET low
  DLY_us(5120);                           // wait a bit
  PIN_low(PIN_SCK);                       // CLK low
  PIN_output(PIN_SCK);                    // CLK to output
  PIN_input(PIN_MISO);                    // MISO not used
  for(i=32; i; i--) TPI_clockBit(1);      // DATA high for 32 TPI clock cycles
}

// Disconnect TPI bus
void TPI_disconnect(void) {
  TPI_writeByte(TPI_OP_SSTCS(TPISR));     // clear NVMEN bit
  TPI_writeByte(0);
  DLY_us(3200);                           // wait a bit
  PIN_high(PIN_RESET);                    // pulse RESET
  DLY_us(1600);
  PIN_low(PIN_RESET);
  DLY_us(1600);
  PIN_input(PIN_RESET);                   // all pins to high impedance
  PIN_input(PIN_SCK);
  PIN_input(PIN_MOSI);
  PIN_input(PIN_MISO);
  ISP_LED_off();                          // turn off status LED
}

// Send raw byte via TPI
void TPI_writeByte(uint8_t b) {
  uint8_t i;
  uint8_t parity = TPI_parity(b);
  TPI_clockBit(0);                        // start bit
  for(i=8; i; i--) {                      // 8 data bits, LSB first
    TPI_clockBit(b & 1);
    b >>= 1;
  }
  TPI_clockBit(parity);                   // parity bit
  TPI_clockBit(1);                        // stop bit
  TPI_clockBit(1);                        // stop bit
}

// Receive raw byte via TPI
uint8_t TPI_readByte(void) {
  uint8_t i, result = 0;

  // Wait for start bit (max 192 TPI clock cycles)
  for(i=192; i; i--) {
    if(!TPI_clockBit(1)) break;
  }
  if(!i) {                                // no start bit found?
    TPI_break();                          // send break
    return 0;
  }

  // Read data bits
  for(i=8; i; i--) {                      // 8 data bits, LSB first
    result >>= 1;
    if(TPI_clockBit(1)) result |= 0x80;
  }

  // Receive and check parity
  if(TPI_clockBit(1) != TPI_parity(result)) {
    TPI_break();                          // parity mismatch: send break
    return 0;
  }

  // Receive stop bits
  TPI_clockBit(1);
  TPI_clockBit(1);
  return result;
}

// ===================================================================================
// TPI Block Functions
// ===================================================================================

// Update pointer register
static void TPI_PR_update(uint16_t pr) {
  TPI_writeByte(TPI_OP_SSTPR(0));
  TPI_writeByte(pr & 0xFF);
  TPI_writeByte(TPI_OP_SSTPR(1));
  TPI_writeByte(pr >> 8);
}

// Read block via TPI into buffer
void TPI_readBlock(uint16_t addr, uint8_t* buf, uint8_t len) {
  TPI_PR_update(addr);
  while(len--) {
    TPI_writeByte(TPI_OP_SLD_INC);
    *buf++ = TPI_readByte();
  }
}

// Write block from buffer via TPI
void TPI_writeBlock(uint16_t addr, uint8_t* buf, uint8_t len) {
  TPI_PR_update(addr);
  while(len--) {
    TPI_writeByte(TPI_OP_SOUT(NVMCMD));
    TPI_writeByte(NVMCMD_WORD_WRITE);
    TPI_writeByte(TPI_OP_SST_INC);
    TPI_writeByte(*buf++);
    do {
      TPI_writeByte(TPI_OP_SIN(NVMCSR));
    } while(TPI_readByte() & NVMCSR_BSY);
  }
}
