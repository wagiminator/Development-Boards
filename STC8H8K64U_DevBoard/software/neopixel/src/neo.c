// ===================================================================================
// NeoPixel (Addressable LED) Functions for STC8H Microcontrollers             * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// The following must be defined in config.h:
// PIN_NEO   - pin connected to DATA-IN of the pixel strip (via a ~330 ohms resistor).
// NEO_GRB   - type of pixel: NEO_GRB or NEO_RGB
// NEO_COUNT - total number of pixels
// System clock frequency must be at least 6 MHz, timing is calculated from F_CPU.
//
// Further information:     https://github.com/wagiminator/ATtiny13-NeoController
// 2023 by Stefan Wagner:   https://github.com/wagiminator

// ===================================================================================
// Libraries, Variables and Constants
// ===================================================================================
#include "neo.h"

#define NEOPIN PIN_asm(PIN_NEO)             // convert PIN_NEO for inline assembly
__xdata uint8_t NEO_buffer[3 * NEO_COUNT];  // pixel buffer
__xdata uint8_t *ptr;                       // pixel buffer pointer

// ===================================================================================
// Protocol Delays
// ===================================================================================
// There are three essential conditions:
// - T0H (HIGH-time for "0"-bit) should be ~300ns (max. 500ns)
// - T1H (HIGH-time for "1"-bit) must be min.  625ns
// - TCT (total clock time) must be      min. 1150ns
// On the STC8H (1T 8051) setb, clr, mov bit and rlc take 1 clock cycle, a taken
// djnz takes 3 clock cycles. The bit transmission loop takes 7 + delay clock cycles.
// The delays (number of nops) are calculated from F_CPU:
#if F_CPU < 6000000
  #error System clock frequency for NeoPixels must be at least 6 MHz!
#endif
#define NEO_T0H_CLK  (((F_CPU * 3 + 5000000) / 10000000) - 1)                // ~300ns
#define NEO_T1H_CLK  (((F_CPU / 100000 * 13 + 199) / 200) - NEO_T0H_CLK - 2) // >=650ns
#define NEO_TCT_RAW  (((F_CPU / 100000 * 5 + 39) / 40) - 7 - NEO_T0H_CLK - NEO_T1H_CLK)
#if NEO_TCT_RAW > 0                                                          // >=1250ns
  #define NEO_TCT_CLK NEO_TCT_RAW
#else
  #define NEO_TCT_CLK 0
#endif

#if (NEO_T0H_CLK & 1)
  #define NEO_T0H_1 \
    nop
#else
  #define NEO_T0H_1
#endif
#if (NEO_T0H_CLK & 2)
  #define NEO_T0H_2 \
    nop \
    nop
#else
  #define NEO_T0H_2
#endif
#if (NEO_T0H_CLK & 4)
  #define NEO_T0H_4 \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T0H_4
#endif
#if (NEO_T0H_CLK & 8)
  #define NEO_T0H_8 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T0H_8
#endif
#if (NEO_T0H_CLK & 16)
  #define NEO_T0H_16 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T0H_16
#endif
#if (NEO_T0H_CLK & 32)
  #define NEO_T0H_32 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T0H_32
#endif
#define NEO_T0H_DELAY \
  NEO_T0H_1 \
  NEO_T0H_2 \
  NEO_T0H_4 \
  NEO_T0H_8 \
  NEO_T0H_16 \
  NEO_T0H_32

#if (NEO_T1H_CLK & 1)
  #define NEO_T1H_1 \
    nop
#else
  #define NEO_T1H_1
#endif
#if (NEO_T1H_CLK & 2)
  #define NEO_T1H_2 \
    nop \
    nop
#else
  #define NEO_T1H_2
#endif
#if (NEO_T1H_CLK & 4)
  #define NEO_T1H_4 \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T1H_4
#endif
#if (NEO_T1H_CLK & 8)
  #define NEO_T1H_8 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T1H_8
#endif
#if (NEO_T1H_CLK & 16)
  #define NEO_T1H_16 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T1H_16
#endif
#if (NEO_T1H_CLK & 32)
  #define NEO_T1H_32 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_T1H_32
#endif
#define NEO_T1H_DELAY \
  NEO_T1H_1 \
  NEO_T1H_2 \
  NEO_T1H_4 \
  NEO_T1H_8 \
  NEO_T1H_16 \
  NEO_T1H_32

#if (NEO_TCT_CLK & 1)
  #define NEO_TCT_1 \
    nop
#else
  #define NEO_TCT_1
#endif
#if (NEO_TCT_CLK & 2)
  #define NEO_TCT_2 \
    nop \
    nop
#else
  #define NEO_TCT_2
#endif
#if (NEO_TCT_CLK & 4)
  #define NEO_TCT_4 \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_TCT_4
#endif
#if (NEO_TCT_CLK & 8)
  #define NEO_TCT_8 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_TCT_8
#endif
#if (NEO_TCT_CLK & 16)
  #define NEO_TCT_16 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_TCT_16
#endif
#if (NEO_TCT_CLK & 32)
  #define NEO_TCT_32 \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop \
    nop
#else
  #define NEO_TCT_32
#endif
#define NEO_TCT_DELAY \
  NEO_TCT_1 \
  NEO_TCT_2 \
  NEO_TCT_4 \
  NEO_TCT_8 \
  NEO_TCT_16 \
  NEO_TCT_32

// ===================================================================================
// Send a Data Byte to the Pixels String
// ===================================================================================
// This is the most time sensitive part. Outside of the function, it must be
// ensured that interrupts are disabled and that the time between the
// transmission of the individual bytes is less than the pixel's latch time.
void NEO_sendByte(uint8_t data) {
  data;                 // stop unreferenced argument warning
  __asm
    mov  r7, #8         ; 1 CLK - 8 bits to transfer
    mov  a, dpl         ; 1 CLK - data byte -> accu
    01$:
    rlc  a              ; 1 CLK - data bit -> carry (MSB first)
    setb NEOPIN         ; 1 CLK - NEO pin HIGH
    NEO_T0H_DELAY       ; x CLK - T0H delay
    mov  NEOPIN, c      ; 1 CLK - "0"-bit? -> NEO pin LOW now
    NEO_T1H_DELAY       ; y CLK - T1H delay
    clr  NEOPIN         ; 1 CLK - "1"-bit? -> NEO pin LOW a little later
    NEO_TCT_DELAY       ; z CLK - TCT delay
    djnz r7, 01$        ; 3 CLK - repeat for all bits
  __endasm;
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint8_t i;
  __bit ea = EA;                            // save interrupt state
  ptr = NEO_buffer;
  EA = 0;                                   // disable interrupts for precise timing
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  EA = ea;                                  // restore interrupt state
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint8_t i;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
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
