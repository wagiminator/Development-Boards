// ===================================================================================
// Basic I2C Master Functions (SERCOM I2CM) for SAMD21                        * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
//                          (call again without stop for a repeated start)
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_read(ack)            I2C receive one data byte (set ack=0 for last byte)
// I2C_stop()               I2C stop transmission
// I2C_busy()               Check if I2C bus is busy
// I2C_nack()               Check if the last address or data byte was not acknowledged
//
// I2C_sendBuffer(addr,buf,len) Send buffer (*buf) with length (len) to device (addr)
// I2C_getBuffer(addr,buf,len)  Receive buffer (*buf) with length (len) from (addr)
// I2C_writeBuffer(buf,len)     Write buffer (*buf) with length (len) via I2C and stop
// I2C_readBuffer(buf,len)      Read buffer (*buf) with length (len) via I2C and stop
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP   0     1     2     3     4     5     6     7     8     9     10    
// SERCOM    3     1     0     2     3     5     2     4     4     5     5     
// SDA-pin   PA22  PA16  PA8   PA8   PA16  PA22  PA12  PA12  PB12  PB16  PB30  No mapping
// SCL-pin   PA23  PA17  PA9   PA9   PA17  PA23  PA13  PA13  PB13  PB17  PB31  No mapping
// Device    EGJ   EGJ   EGJ   EGJ   EGJ   GJ    GJ    GJ    J     J     J     
//
// Notes:
// ------
// - Device row: mapping available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
// - Only pins with I2C capability are used (PA8/9, PA12/13, PA16/17, PA22/23, PB12/13,
//   PB16/17, PB30/31). Mapping 0 corresponds to the Arduino Zero (Wire).
// - I2C clock rates up to 400kHz use Standard/Fast-mode, above Fast-mode Plus.
//   The SERCOM is clocked by GCLK0 (F_CPU), the minimum clock rate at 48MHz is ~95kHz
//   (use a lower F_CPU or the i2c_soft library for lower clock rates).
// - If the slave does not acknowledge (NACK), the transfer is aborted (STOP), all
//   further writes are ignored and reads return 0xff until the next I2C_start().
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C parameters
#define I2C_CLKRATE       400000  // I2C bus clock rate (Hz)
#define I2C_MAP           1       // I2C pin mapping (see above)

// I2C pin mapping (SERCOM, SDA/SCL pin, peripheral function)
#if   I2C_MAP == 0
  #define I2C_SERCOM        3
  #define I2C_SDA_PIN       22          // PA22
  #define I2C_SCL_PIN       23          // PA23
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 1
  #define I2C_SERCOM        1
  #define I2C_SDA_PIN       16          // PA16
  #define I2C_SCL_PIN       17          // PA17
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 2
  #define I2C_SERCOM        0
  #define I2C_SDA_PIN       8           // PA8
  #define I2C_SCL_PIN       9           // PA9
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 3
  #define I2C_SERCOM        2
  #define I2C_SDA_PIN       8           // PA8
  #define I2C_SCL_PIN       9           // PA9
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 4
  #define I2C_SERCOM        3
  #define I2C_SDA_PIN       16          // PA16
  #define I2C_SCL_PIN       17          // PA17
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 5
  #define I2C_SERCOM        5
  #define I2C_SDA_PIN       22          // PA22
  #define I2C_SCL_PIN       23          // PA23
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 6
  #define I2C_SERCOM        2
  #define I2C_SDA_PIN       12          // PA12
  #define I2C_SCL_PIN       13          // PA13
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 7
  #define I2C_SERCOM        4
  #define I2C_SDA_PIN       12          // PA12
  #define I2C_SCL_PIN       13          // PA13
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#elif I2C_MAP == 8
  #define I2C_SERCOM        4
  #define I2C_SDA_PIN       44          // PB12
  #define I2C_SCL_PIN       45          // PB13
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 9
  #define I2C_SERCOM        5
  #define I2C_SDA_PIN       48          // PB16
  #define I2C_SCL_PIN       49          // PB17
  #define I2C_SDA_PMUX      2           // C
  #define I2C_SCL_PMUX      2           // C
#elif I2C_MAP == 10
  #define I2C_SERCOM        5
  #define I2C_SDA_PIN       62          // PB30
  #define I2C_SCL_PIN       63          // PB31
  #define I2C_SDA_PMUX      3           // D
  #define I2C_SCL_PMUX      3           // D
#else
  #define I2C_SERCOM        3           // pins must be set by the application
#endif

#if (I2C_MAP == 5) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 6) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 7) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 8) && !(defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 9) && !(defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (I2C_MAP == 10) && !(defined(SAMD21J))
  #error Pin mapping not available on this device
#endif

#if (I2C_SERCOM > 3) && defined(SAMD21E)
  #error SERCOM4 and SERCOM5 are not available on SAMD21E
#endif

#if   I2C_SERCOM == 0
  #define I2C_SER            SERCOM0
  #define I2C_GCLK_ID        SERCOM0_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM0
  #define I2C_IRQn           SERCOM0_IRQn
  #define I2C_IRQHandler     SERCOM0_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM0_DMAC_ID_TX
#elif I2C_SERCOM == 1
  #define I2C_SER            SERCOM1
  #define I2C_GCLK_ID        SERCOM1_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM1
  #define I2C_IRQn           SERCOM1_IRQn
  #define I2C_IRQHandler     SERCOM1_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM1_DMAC_ID_TX
#elif I2C_SERCOM == 2
  #define I2C_SER            SERCOM2
  #define I2C_GCLK_ID        SERCOM2_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM2
  #define I2C_IRQn           SERCOM2_IRQn
  #define I2C_IRQHandler     SERCOM2_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM2_DMAC_ID_TX
#elif I2C_SERCOM == 3
  #define I2C_SER            SERCOM3
  #define I2C_GCLK_ID        SERCOM3_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM3
  #define I2C_IRQn           SERCOM3_IRQn
  #define I2C_IRQHandler     SERCOM3_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM3_DMAC_ID_TX
#elif I2C_SERCOM == 4
  #define I2C_SER            SERCOM4
  #define I2C_GCLK_ID        SERCOM4_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM4
  #define I2C_IRQn           SERCOM4_IRQn
  #define I2C_IRQHandler     SERCOM4_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM4_DMAC_ID_TX
#elif I2C_SERCOM == 5
  #define I2C_SER            SERCOM5
  #define I2C_GCLK_ID        SERCOM5_GCLK_ID_CORE
  #define I2C_APBMASK        PM_APBCMASK_SERCOM5
  #define I2C_IRQn           SERCOM5_IRQn
  #define I2C_IRQHandler     SERCOM5_IRQHandler
  #define I2C_DMAC_ID_TX     SERCOM5_DMAC_ID_TX
#endif

// I2C functions
void I2C_init(void);              // I2C init function
void I2C_start(uint8_t addr);     // I2C start transmission, addr must contain R/W bit
void I2C_stop(void);              // I2C stop transmission
void I2C_write(uint8_t data);     // I2C transmit one data byte via I2C
uint8_t I2C_read(uint8_t ack);    // I2C receive one data byte from the slave
uint8_t I2C_nack(void);           // check if last byte was not acknowledged

void I2C_writeBuffer(uint8_t* buf, uint16_t len);
void I2C_readBuffer(uint8_t* buf, uint16_t len);

#define I2C_sendBuffer(addr,buf,len)  (I2C_start(addr), I2C_writeBuffer(buf,len))
#define I2C_getBuffer(addr,buf,len)   (I2C_start(addr), I2C_readBuffer(buf,len))

#define I2C_busy()        ((I2C_SER->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk) \
                          > SERCOM_I2CM_STATUS_BUSSTATE(1))

#ifdef __cplusplus
};
#endif
