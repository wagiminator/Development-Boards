// ===================================================================================
// Basic PRINT Functions                                                      * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// printF(putchar, f, ...)  printf (supports %s, %c, %d, %u, %x, %b, %02d, %%, and
//                          the length modifier l for 32-bit values, e.g. %ld, %lu)
// printD(putchar, n)       Print 16-bit decimal value as string via putchar function
// printW(putchar, n)       Print 32-bit hex word value as string via putchar function
// printH(putchar, n)       Print 16-bit hex half-word value as string via putchar function
// printB(putchar, n)       Print  8-bit hex byte value as string via putchar function
// printS(putchar, s)       Print string via putchar function
// println(putchar, s)      Print string with newline via putchar function
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void printD(void (*putchr) (char c), uint16_t value);
void printB(void (*putchr) (char c), uint8_t value);
void printH(void (*putchr) (char c), uint16_t value);
void printW(void (*putchr) (char c), uint32_t value);
void printS(void (*putchr) (char c), const char* str);
void println(void (*putchr) (char c), const char* str);
void printF(void (*putchr) (char c), const char *format, ...);

#ifdef __cplusplus
};
#endif
