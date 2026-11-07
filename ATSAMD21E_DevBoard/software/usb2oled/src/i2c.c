// ===================================================================================
// Basic I2C Master Functions (SERCOM I2CM) for SAMD21                        * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c.h"
#include "gpio.h"

// I2C BAUD value: SCL = F_CPU / (10 + 2 * BAUD + F_CPU * t_rise), t_rise ~ 125ns
#define I2C_BAUD_VAL      (F_CPU / (2 * I2C_CLKRATE) - 5 - (F_CPU / 1000000 * 125) / 2000)

#if I2C_BAUD_VAL > 255
  #error I2C_CLKRATE too low for F_CPU
#endif
#if I2C_BAUD_VAL < 1
  #error I2C_CLKRATE too high for F_CPU
#endif

// I2C states
#define I2C_ST_IDLE       0       // no transfer
#define I2C_ST_OPEN       1       // transfer open (bus owner)
#define I2C_ST_NACK       2       // transfer aborted (NACK, error)
static uint8_t I2C_state = I2C_ST_IDLE;

// Wait for synchronization of system operations
#define I2C_sync()        while(I2C_SER->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_SYSOP)

// Send STOP condition and wait until bus is idle
static void I2C_sendStop(void) {
  I2C_SER->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);    // (NACK and) STOP
  I2C_sync();
  while((I2C_SER->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk)
        == SERCOM_I2CM_STATUS_BUSSTATE(2));               // wait until not owner
}

// Wait for MB or SB flag; on NACK or error abort transfer and return 0
static uint8_t I2C_wait(void) {
  while(!(I2C_SER->I2CM.INTFLAG.reg & (SERCOM_I2CM_INTFLAG_MB | SERCOM_I2CM_INTFLAG_SB)));
  if(I2C_SER->I2CM.STATUS.reg & (SERCOM_I2CM_STATUS_RXNACK | SERCOM_I2CM_STATUS_ARBLOST
                               | SERCOM_I2CM_STATUS_BUSERR)) {
    if(!(I2C_SER->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST)) I2C_sendStop();
    I2C_state = I2C_ST_NACK;
    return 0;
  }
  return 1;
}

// Init I2C
void I2C_init(void) {
  // Set GPIO pins (internal pullups enabled, external pullups still mandatory)
  #ifdef I2C_SDA_PMUX
  PIN_input_PU(I2C_SDA_PIN);
  PIN_alternate(I2C_SDA_PIN, I2C_SDA_PMUX);
  PIN_input_PU(I2C_SCL_PIN);
  PIN_alternate(I2C_SCL_PIN, I2C_SCL_PMUX);
  #else
    #warning No automatic pin mapping for I2C
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= I2C_APBMASK;                        // enable SERCOM module clock
  GCLK_setCH(I2C_GCLK_ID, GCLK_SYS);                      // GCLK0 -> SERCOM

  // Setup and enable I2C master
  I2C_SER->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;
  while(I2C_SER->I2CM.CTRLA.reg & SERCOM_I2CM_CTRLA_SWRST);
  I2C_SER->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER  // I2C master mode
                          #if I2C_CLKRATE > 400000
                          | SERCOM_I2CM_CTRLA_SPEED(1)         // Fast-mode Plus
                          #endif
                          | SERCOM_I2CM_CTRLA_SDAHOLD(2);      // SDA hold 300-600ns
  I2C_SER->I2CM.CTRLB.reg = 0;                            // no smart mode
  I2C_SER->I2CM.BAUD.reg  = SERCOM_I2CM_BAUD_BAUD(I2C_BAUD_VAL);
  I2C_SER->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;    // enable I2C
  while(I2C_SER->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_ENABLE);
  I2C_SER->I2CM.STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);  // force bus state IDLE
  I2C_sync();
  I2C_state = I2C_ST_IDLE;
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  if(I2C_state != I2C_ST_OPEN) {                          // no repeated start?
    while(I2C_busy());                                    // wait until bus is free
  }
  I2C_SER->I2CM.ADDR.reg = SERCOM_I2CM_ADDR_ADDR(addr);   // send (repeated) START + addr
  I2C_sync();
  I2C_state = I2C_ST_OPEN;
  I2C_wait();                                             // wait for address sent
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  if(I2C_state != I2C_ST_OPEN) return;                    // no open transfer (or NACK)
  I2C_SER->I2CM.DATA.reg = data;                          // send data byte
  I2C_sync();
  I2C_wait();                                             // wait for data byte sent
}

// Read data byte via I2C bus (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t I2C_read(uint8_t ack) {
  uint8_t data;
  if(I2C_state != I2C_ST_OPEN) return 0xff;               // no open transfer (or NACK)
  while(!(I2C_SER->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)); // wait for data byte
  data = I2C_SER->I2CM.DATA.reg;                          // read data byte
  if(ack) {                                               // more bytes to follow?
    I2C_SER->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_CMD(2);   // send ACK, read next byte
    I2C_sync();
  }
  else I2C_SER->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_ACKACT;// NACK with STOP/Sr
  return data;
}

// Stop I2C transmission
void I2C_stop(void) {
  if(I2C_state == I2C_ST_OPEN) I2C_sendStop();            // send STOP condition
  I2C_SER->I2CM.CTRLB.reg = 0;                            // ACKACT back to ACK
  I2C_state = I2C_ST_IDLE;                                // close transfer
}

// Check if the last address or data byte was not acknowledged
uint8_t I2C_nack(void) {
  return (I2C_state == I2C_ST_NACK);
}

// Send data buffer via I2C bus and stop
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(len--) I2C_write(*buf++);                         // write buffer
  I2C_stop();                                             // stop transmission
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);                // read buffer
  I2C_stop();                                             // stop transmission
}
