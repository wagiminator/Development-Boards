// ===================================================================================
// Basic MAX7219/7221 Segment/Matrix LED Display Functions                    * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "max7219.h"

// MAX7219 send command
void MAX_send(uint8_t reg, uint8_t data) {
  PIN_low(MAX_PIN_CS);                            // set CS low  -> select device
  SPI_transfer(reg);                              // send address byte
  SPI_transfer(data);                             // send data byte
  PIN_high(MAX_PIN_CS);                           // set CS high -> latch the bytes
}

// Init MAX7219 (internal)
static void MAX_init(uint8_t decode, uint8_t limit) {
  PIN_high(MAX_PIN_CS);                           // set CS high (unselect device)
  PIN_output(MAX_PIN_CS);                         // set CS pin to output
  #if MAX_INIT_SPI > 0
  SPI_init();                                     // init SPI
  #endif
  MAX_send(MAX_ADDR_TEST,   0x00);                // display test off
  MAX_send(MAX_ADDR_MODE,   decode);              // set decode mode
  MAX_send(MAX_ADDR_BRIGHT, MAX_BRIGHT);          // set intensity (0x00 .. 0x0f)
  MAX_send(MAX_ADDR_LIMIT,  limit);               // set scan limit (number of digits)
  MAX_send(MAX_ADDR_SHTDWN, 0x01);                // shutdown mode off
}

// ===================================================================================
// Segment Display Functions
// ===================================================================================

// Init MAX7219 for segment display
void SEG_init(void) {
  MAX_init(0xff, SEG_COUNT - 1);                  // BCD decode for all digits
  SEG_clear();
}

// Clear all digits
void SEG_clear(void) {
  uint8_t i;
  for(i = 1; i <= SEG_COUNT; i++) MAX_send(i, SEG_CODE_BLANK);
}

// Print a decimal value on segment display (with leading zeros)
void SEG_printD(uint32_t value) {
  uint8_t i;
  for(i = 1; i <= SEG_COUNT; i++, value /= 10) MAX_send(i, value % 10);
}

// Print value with decimal at dotpos on segment display (without leading zeros)
void SEG_print(int32_t value, uint8_t dotpos) {
  uint8_t  digit = 1;                             // start with rightmost digit
  uint8_t  neg   = 0;
  uint32_t v;
  if(value < 0) {                                 // negative value?
    neg = 1;
    v = -(uint32_t)value;
  }
  else v = value;
  do {                                            // print digits from right to left
    uint8_t code = v % 10;
    v /= 10;
    if(digit - 1 == dotpos && dotpos) code |= SEG_CODE_DOT;
    MAX_send(digit++, code);
  } while((v || (dotpos < SEG_COUNT && digit <= dotpos + 1)) && digit <= SEG_COUNT);
  if(neg && digit <= SEG_COUNT) MAX_send(digit++, SEG_CODE_NEG);
  while(digit <= SEG_COUNT) MAX_send(digit++, SEG_CODE_BLANK);
}

// ===================================================================================
// Matrix Display Functions
// ===================================================================================

// Init MAX7219 for matrix display
void MTX_init(void) {
  MAX_init(0x00, 7);                              // no decode, 8 rows
  MTX_clear();
}

// Clear matrix
void MTX_clear(void) {
  uint8_t i;
  for(i = 1; i <= 8; i++) MAX_send(i, 0);
}

// Draw 8x8 bitmap (one byte per row)
void MTX_drawBitmap(const uint8_t* bmp) {
  uint8_t i;
  for(i = 1; i <= 8; i++) MAX_send(i, *bmp++);
}
