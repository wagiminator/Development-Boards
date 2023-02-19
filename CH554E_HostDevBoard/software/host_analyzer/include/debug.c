// ===================================================================================
// Serial Debug Functions for CH551, CH552 and CH554                          * v1.0 *
// ===================================================================================
//
// Basic UART TX debug functions. Use printf commands!
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "debug.h"

#if DEBUG_PORT == 0

#if SDCC < 370
void putchar(char c) {
  while (!TI);
  TI = 0;
  SBUF = c;
}
#else
int putchar(int c) {
  while (!TI);
  TI = 0;
  SBUF = c & 0xFF;
  return c;
}
#endif

#elif DEBUG_PORT == 1

#if SDCC < 370
void putchar(char c) {
  while(!U1TI);
  U1TI = 0;
  SBUF1 = c;
}
#else
int putchar(int c) {
  while(!U1TI);
  U1TI = 0;
  SBUF1 = c & 0xFF;
  return c;
}
#endif

#endif
