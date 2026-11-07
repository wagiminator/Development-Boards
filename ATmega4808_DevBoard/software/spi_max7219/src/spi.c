// ===================================================================================
// Basic SPI Master Functions for megaAVR 0-Series                            * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "spi.h"

// SPI init
void SPI_init(void) {
  #if SPI_MAP == 0
    VPORTA.DIR |= (1<<4) | (1<<6);              // MOSI (PA4), SCK (PA6) as output
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;         // pullup on MISO (PA5)
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm) | PORTMUX_SPI0_DEFAULT_gc;
  #elif SPI_MAP == 1
    VPORTC.DIR |= (1<<0) | (1<<2);              // MOSI (PC0), SCK (PC2) as output
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;         // pullup on MISO (PC1)
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm) | PORTMUX_SPI0_ALT1_gc;
  #elif SPI_MAP == 2 && defined(TCB3)
    VPORTE.DIR |= (1<<0) | (1<<2);              // MOSI (PE0), SCK (PE2) as output
    PORTE.PIN1CTRL |= PORT_PULLUPEN_bm;         // pullup on MISO (PE1)
    PORTMUX.TWISPIROUTEA = (PORTMUX.TWISPIROUTEA & ~PORTMUX_SPI0_gm) | PORTMUX_SPI0_ALT2_gc;
  #else
    #warning No automatic pin mapping for SPI0
  #endif

  SPI0.CTRLA = SPI_CLK2X_bm                     // double speed
             | SPI_ENABLE_bm                    // enable SPI
             | SPI_MASTER_bm                    // master mode
             | (SPI_PRESC << SPI_PRESC_gp);     // prescaler
  SPI0.CTRLB = SPI_SSD_bm;                      // disable SS line
}

// SPI transmit and receive a byte
uint8_t SPI_transfer(uint8_t data) {
  SPI0.DATA = data;                             // start exchanging data byte
  while(~SPI0.INTFLAGS & SPI_IF_bm);            // wait for transfer to complete
  return SPI0.DATA;                             // return received byte
}
