// ===================================================================================
// Basic SPI Master Functions for tinyAVR 0-Series and 1-Series               * v1.1 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "spi.h"

// SPI init
void SPI_init(void) {
  #if SPI_MAP == 0
    VPORTA.DIR |= (1<<1);
    VPORTA.DIR |= (1<<3);
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
  #elif SPI_MAP == 1 && defined(PORTC)
    VPORTC.DIR |= (1<<2);
    VPORTC.DIR |= (1<<0);
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTMUX.CTRLB  |= PORTMUX_SPI0_bm;
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
