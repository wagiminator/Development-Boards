// ===================================================================================
// Basic PRINT Functions                                                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// printD(n, putchar)       Print decimal value as string via putchar function
// printW(n, putchar)       Print 32-bit hex word value as string via putchar function
// printH(n, putchar)       Print 16-bit hex half-word value as string via putchar function
// printB(n, putchar)       Print  8-bit hex byte value as string via putchar function
// printS(s, putchar)       Print string via putchar function
// println(s, putchar)      Print string with newline via putchar function
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void printD(uint32_t value, void (*putchar) (char c));
void printB(uint8_t value, void (*putchar) (char c));
void printH(uint16_t value, void (*putchar) (char c));
void printW(uint32_t value, void (*putchar) (char c));
void printS(const char* str, void (*putchar) (char c));
void println(const char* str, void (*putchar) (char c));

#ifdef __cplusplus
};
#endif
