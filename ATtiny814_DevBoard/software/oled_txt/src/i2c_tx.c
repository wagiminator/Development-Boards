// ===================================================================================
// Basic I2C Master Functions (write only) for tinyAVR 0-Series and 1-Series  * v1.3 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_tx.h"

// Missing in some device headers (0-series)
#ifndef PORTMUX_TWI0_bm
  #define PORTMUX_TWI0_bm 0x10
#endif

// BAUD calculation: f_SCL = F_CPU / (10 + 2 * BAUD + F_CPU * t_rise), t_rise ~ 250ns
#define I2C_BAUD_CALC   (((F_CPU / I2C_CLKRATE) - 10 - (F_CPU / 4000000)) / 2)
#if I2C_BAUD_CALC < 1
  #define I2C_BAUD      1                         // fastest possible at low F_CPU
#else
  #define I2C_BAUD      I2C_BAUD_CALC
#endif

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    PORTMUX.CTRLB &= ~PORTMUX_TWI0_bm;
  #elif I2C_MAP == 1 && defined(PORTB)
    PORTMUX.CTRLB |= PORTMUX_TWI0_bm;
  #else
    #warning No automatic pin mapping for TWI0
  #endif

  // Setup and enable I2C
  TWI0.MBAUD   = I2C_BAUD;                      // set TWI master BAUD rate
  TWI0.MCTRLA  = TWI_ENABLE_bm;                 // enable TWI master
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;          // set bus state to idle
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  TWI0.MADDR = addr;                            // start sending address
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  TWI0.MDATA = data;                            // start sending data byte 
}

// Stop I2C transmission
void I2C_stop(void) {
  while(~TWI0.MSTATUS & TWI_WIF_bm);            // wait for last transfer to complete
  TWI0.MCTRLB = TWI_MCMD_STOP_gc;               // send stop condition
}

// Send data buffer via I2C bus and stop
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(len--) I2C_write(*buf++);           // write buffer
  I2C_stop();                               // stop transmission
}
