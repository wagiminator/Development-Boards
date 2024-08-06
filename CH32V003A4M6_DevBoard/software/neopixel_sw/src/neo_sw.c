// ===================================================================================
// Basic NeoPixel Functions using Software Bit-banging for CH32V003           * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_sw.h"

// ===================================================================================
// Neopixel Parameters and Variables
// ===================================================================================

// Define protocol delays depending on system frequency (F_CPU)
// There are three essential conditions:
// - T0H (HIGH-time for "0"-bit) must be max.  500ns
// - T1H (HIGH-time for "1"-bit) must be min.  625ns
// - TCT (total clock time) must be      min. 1150ns
#if F_CPU == 48000000
  #define NEO_DLY1  " c.li a1, 3 \n3: \n c.addi a1, -1 \n c.bnez a1, 3b \n"
  #define NEO_DLY2  " c.li a1, 3 \n4: \n c.addi a1, -1 \n c.bnez a1, 4b \n"
  #define NEO_DLY3  " c.li a1, 3 \n5: \n c.addi a1, -1 \n c.bnez a1, 5b \n"
#elif F_CPU == 24000000
  #define NEO_DLY1  " c.li a1, 1 \n3: \n c.addi a1, -1 \n c.bnez a1, 3b \n"
  #define NEO_DLY2  " c.li a1, 3 \n4: \n c.addi a1, -1 \n c.bnez a1, 4b \n"
  #define NEO_DLY3  " c.li a1, 1 \n5: \n c.addi a1, -1 \n c.bnez a1, 5b \n"
#elif F_CPU == 12000000
  #define NEO_DLY1  " c.nop \n"
  #define NEO_DLY2  " c.nop \n c.nop \n"
  #define NEO_DLY3
#elif F_CPU == 8000000
  #define NEO_DLY1
  #define NEO_DLY2  " c.nop \n"
  #define NEO_DLY3
#else
  #error Unsupported system frequency for NeoPixels!
#endif

// Define some constants depending on the NeoPixel pin
#define NEO_GPIO_BASE \
  ((PIN_NEO>=PA0)&&(PIN_NEO<=PA7) ? ( GPIOA_BASE ) : \
  ((PIN_NEO>=PC0)&&(PIN_NEO<=PC7) ? ( GPIOC_BASE ) : \
  ((PIN_NEO>=PD0)&&(PIN_NEO<=PD7) ? ( GPIOD_BASE ) : \
(0))))
#define NEO_GPIO_BSHR   0x10
#define NEO_GPIO_BCR    0x14
#define NEO_PIN_BM      (1<<((PIN_NEO)&7))

// NeoPixel buffer
uint8_t NEO_buffer[3 * NEO_COUNT];          // pixel buffer

// ===================================================================================
// Init Neopixels
// ===================================================================================
void NEO_init(void) {
  PIN_output(PIN_NEO);
  NEO_latch();
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
// This is the most time sensitive part. Outside of the function, it must be 
// ensured that interrupts are disabled and that the time between the 
// transmission of the individual bytes is less than the pixel's latch time.
void NEO_sendByte(uint8_t data) {
  asm volatile(
    " c.li a5, 8                \n"   // 8 bits to shift out
    " li a4, %[pin]             \n"   // neopixel pin bitmap (compressed for pins 0-4)
    " li a3, %[base]            \n"   // GPIO base address   (single instr for port C)
    "1:                         \n"
    " andi a2, %[byte], 0x80    \n"   // mask bit to shift (MSB first)
    " c.sw a4, %[bshr](a3)      \n"   // set neopixel pin HIGH
    NEO_DLY1                          // T0H delay
    " c.bnez a2, 2f             \n"   // skip next instruction if bit = "1"
    " c.sw a4, %[bcr](a3)       \n"   // bit = "0" : set pin LOW after <= 500ns
    "2:                         \n"
    NEO_DLY2                          // T1H delay
    " c.sw a4, %[bcr](a3)       \n"   // bit = "1" : set pin LOW after >= 625ns
    NEO_DLY3                          // TCT delay
    " c.slli %[byte], 1         \n"   // shift for next bit
    " c.addi a5, -1             \n"   // decrease bit counter
    " c.bnez a5, 1b             \n"   // repeat for 8 bits
    :
    [byte] "+r" (data)
    :
    [pin]  "i"  (NEO_PIN_BM),
    [base] "i"  (NEO_GPIO_BASE),
    [bshr] "i"  (NEO_GPIO_BSHR),
    [bcr]  "i"  (NEO_GPIO_BCR)
    :
    "a1", "a2", "a3", "a4", "a5", "memory"
  );
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint8_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint8_t i;
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
