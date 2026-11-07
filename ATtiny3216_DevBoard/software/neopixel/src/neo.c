// ===================================================================================
// NeoPixel (Addressable LED) Functions for tinyAVR 0/1-Series                * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "neo.h"

#if F_CPU < 8000000
  #error NeoPixel library needs F_CPU >= 8MHz
#endif

// ===================================================================================
// Bit Timing (in clock cycles)
// ===================================================================================
// T0H ~ 350ns, T1H ~ 800ns, bit period ~ 1250ns
#define NEO_CYC(ns)   ((((F_CPU / 1000000UL) * (ns)) + 500) / 1000)
#define NEO_D1        (NEO_CYC(350)  - 3)                   // high time for 0-bit
#define NEO_D2        (NEO_CYC(800)  - NEO_D1 - 5)          // high time for 1-bit
#define NEO_D3        (NEO_CYC(1250) - NEO_D1 - NEO_D2 - 8) // rest of bit period
#if NEO_D3 < 0
  #undef  NEO_D3
  #define NEO_D3      0
#endif

// VPORT OUT register (I/O address) and bit of data pin
#define NEO_PORT      ((((NEO_PIN) >> 3) << 2) + 1)
#define NEO_BIT       ((NEO_PIN) & 7)

// ===================================================================================
// Send a Data Byte to the Pixels String (MSB first)
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t bits;
  uint8_t sreg = SREG;                          // save interrupt status
  cli();                                        // disable interrupts
  asm volatile (
    "  ldi  %[bits], 8          \n"             // 8 bits to send
    "1:                         \n"
    "  sbi  %[port], %[bit]     \n"             // 1 cycle:  pin HIGH
    "  .rept %[d1]              \n"
    "  nop                      \n"             // D1 cycles
    "  .endr                    \n"
    "  sbrs %[data], 7          \n"             // 1/2 cycles: skip if bit is 1
    "  cbi  %[port], %[bit]     \n"             // 1 cycle:  pin LOW (0-bit)
    "  lsl  %[data]             \n"             // 1 cycle:  next bit
    "  .rept %[d2]              \n"
    "  nop                      \n"             // D2 cycles
    "  .endr                    \n"
    "  cbi  %[port], %[bit]     \n"             // 1 cycle:  pin LOW (1-bit)
    "  .rept %[d3]              \n"
    "  nop                      \n"             // D3 cycles
    "  .endr                    \n"
    "  dec  %[bits]             \n"             // 1 cycle:  decrease bit counter
    "  brne 1b                  \n"             // 2 cycles: repeat for all bits
    : [bits] "=&d" (bits), [data] "+r" (data)
    : [port] "I" (NEO_PORT), [bit] "I" (NEO_BIT),
      [d1] "n" (NEO_D1), [d2] "n" (NEO_D2), [d3] "n" (NEO_D3)
  );
  SREG = sreg;                                  // restore interrupt status
}

// ===================================================================================
// Pixel Buffer and Functions
// ===================================================================================
static uint8_t NEO_buffer[3 * NEO_COUNT];       // pixel buffer (G, R, B / R, G, B)

// Write buffer to pixels
void NEO_update(void) {
  uint8_t* ptr = NEO_buffer;
  for(uint16_t i = 3 * NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  NEO_latch();
}

// Clear all pixels and update
void NEO_clearAll(void) {
  uint8_t* ptr = NEO_buffer;
  for(uint16_t i = 3 * NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// Clear one pixel
void NEO_clearPixel(uint8_t pixel) {
  NEO_writeColor(pixel, 0, 0, 0);
}

// Write color to a single pixel
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  if(pixel >= NEO_COUNT) return;
  uint8_t* ptr = &NEO_buffer[3 * pixel];
  #if defined(NEO_GRB)
  *ptr++ = g; *ptr++ = r; *ptr = b;
  #elif defined(NEO_RGB)
  *ptr++ = r; *ptr++ = g; *ptr = b;
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// Write hue value (0..191) and brightness (0..2) to a single pixel
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
