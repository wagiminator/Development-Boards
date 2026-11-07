// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI (SERCOM) for SAMD11           * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_spi.h"
#include "gpio.h"

// ===================================================================================
// SPI Parameters and Variables
// ===================================================================================

// Define SPI clock (SCK = F_CPU / (2 * (NEO_BAUD + 1))) and bit patterns (MSB first)
// for "0"-bit and "1"-bit depending on system frequency:
// - 6MHz SPI: 166.7ns per SPI bit, "0": 333ns HIGH, "1": 833ns HIGH, 1333ns total
// - 8MHz SPI: 125.0ns per SPI bit, "0": 375ns HIGH, "1": 750ns HIGH, 1000ns total
#if   F_CPU == 48000000
  #define NEO_BAUD    3
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 24000000
  #define NEO_BAUD    1
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 16000000
  #define NEO_BAUD    0
  #define NEO_BIT0    0x70
  #define NEO_BIT1    0x7E
#elif F_CPU == 12000000
  #define NEO_BAUD    0
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#else
  #error Unsupported system frequency for NeoPixels!
#endif

// SPI pin mapping (SERCOM, MOSI pin, peripheral function)
#if   NEO_MAP == 0
  #define NEO_SERCOM         0
  #define NEO_SCK_PIN        5
  #define NEO_MOSI_PIN       4
  #define NEO_PMUX           2           // C
  #define NEO_DIPO           0           // MISO on PAD0
#elif   NEO_MAP == 1
  #define NEO_SERCOM         0
  #define NEO_SCK_PIN        9
  #define NEO_MOSI_PIN       8
  #define NEO_PMUX           3           // D
  #define NEO_DIPO           1           // MISO on PAD1
#elif   NEO_MAP == 2
  #define NEO_SERCOM         1
  #define NEO_SCK_PIN        9
  #define NEO_MOSI_PIN       8
  #define NEO_PMUX           2           // C
  #define NEO_DIPO           1           // MISO on PAD1
#elif   NEO_MAP == 3
  #define NEO_SERCOM         0
  #define NEO_SCK_PIN        7
  #define NEO_MOSI_PIN       6
  #define NEO_PMUX           3           // D
  #define NEO_DIPO           1           // MISO on PAD1
#elif   NEO_MAP == 4
  #define NEO_SERCOM         1
  #define NEO_SCK_PIN        17
  #define NEO_MOSI_PIN       16
  #define NEO_PMUX           2           // C
  #define NEO_DIPO           0           // MISO on PAD0
#elif   NEO_MAP == 5
  #define NEO_SERCOM         2
  #define NEO_SCK_PIN        11
  #define NEO_MOSI_PIN       10
  #define NEO_PMUX           3           // D
  #define NEO_DIPO           0           // MISO on PAD0
#else
  #define NEO_SERCOM         0           // pins must be set by the application
  #define NEO_DIPO           0
#endif

#if defined(SAMD11C) && (NEO_MAP >= 3) && (NEO_MAP <= 5)
  #error Pin mapping not available on SAMD11C
#endif

#if (NEO_SERCOM == 2) && defined(SAMD11C)
  #error SERCOM2 is not available on SAMD11C
#endif

#if   NEO_SERCOM == 0
  #define NEO_SER            SERCOM0
  #define NEO_GCLK_ID        SERCOM0_GCLK_ID_CORE
  #define NEO_APBMASK        PM_APBCMASK_SERCOM0
  #define NEO_DMAC_ID_TX     SERCOM0_DMAC_ID_TX
#elif NEO_SERCOM == 1
  #define NEO_SER            SERCOM1
  #define NEO_GCLK_ID        SERCOM1_GCLK_ID_CORE
  #define NEO_APBMASK        PM_APBCMASK_SERCOM1
  #define NEO_DMAC_ID_TX     SERCOM1_DMAC_ID_TX
#else
  #define NEO_SER            SERCOM2
  #define NEO_GCLK_ID        SERCOM2_GCLK_ID_CORE
  #define NEO_APBMASK        PM_APBCMASK_SERCOM2
  #define NEO_DMAC_ID_TX     SERCOM2_DMAC_ID_TX
#endif


#define NEO_DATA      (NEO_SER->SPI.DATA.reg)
#define NEO_DRE       (NEO_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)
#define NEO_TXC       (NEO_SER->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC)

// NeoPixel data buffer
uint8_t NEO_buffer[3 * NEO_COUNT];

// ===================================================================================
// Init SPI for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Set GPIO pin
  #ifdef NEO_PMUX
  PIN_low(NEO_MOSI_PIN);
  PIN_output(NEO_MOSI_PIN);
  PIN_alternate(NEO_MOSI_PIN, NEO_PMUX);
  #else
    #warning No automatic pin mapping for SPI
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= NEO_APBMASK;              // enable SERCOM module clock
  GCLK_setCH(NEO_GCLK_ID, GCLK_SYS);            // GCLK0 -> SERCOM

  // Setup and enable SPI master, TX only, MSB first, 8-bit
  NEO_SER->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_SWRST;
  while(NEO_SER->SPI.CTRLA.reg & SERCOM_SPI_CTRLA_SWRST);
  NEO_SER->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER   // master mode
                         | SERCOM_SPI_CTRLA_DOPO(1);          // MOSI PAD2, SCK PAD3
  NEO_SER->SPI.CTRLB.reg = 0;                                 // receiver disabled
  while(NEO_SER->SPI.SYNCBUSY.reg);
  NEO_SER->SPI.BAUD.reg  = NEO_BAUD;                          // set clock rate
  NEO_SER->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;          // enable SPI
  while(NEO_SER->SPI.SYNCBUSY.reg);
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t i;
  for(i=8; i; i--, data<<=1) {              // 8 bits, MSB first
    while(!NEO_DRE);                        // wait for transmit buffer empty
    if(data & 0x80) NEO_DATA = NEO_BIT1;    // long HIGH pulse for "1"-bit
    else            NEO_DATA = NEO_BIT0;    // short HIGH pulse for "0"-bit
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  while(!NEO_TXC);                          // wait for last byte transmitted
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *ptr;
  ptr = NEO_buffer + (3 * pixel);
  #if defined (NEO_GRB)
    *ptr++ = g; *ptr++ = r; *ptr = b;
  #elif defined (NEO_RGB)
    *ptr++ = r; *ptr++ = g; *ptr = b;
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// ===================================================================================
// Write Hue Value (0..191) and Brightness (0..2) to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   NEO_writeColor(pixel, nstep,  step,     0); break;
    case 1:   NEO_writeColor(pixel,     0, nstep,  step); break;
    case 2:   NEO_writeColor(pixel,  step,     0, nstep); break;
    default:  break;
  }
}

// ===================================================================================
// Clear Single Pixel in Buffer
// ===================================================================================
void NEO_clearPixel(uint8_t pixel) {
  NEO_writeColor(pixel, 0, 0, 0);
}
