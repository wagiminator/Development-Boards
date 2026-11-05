// ===================================================================================
// Minimal C Library Functions (the project is compiled with -nostdlib)
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include <stddef.h>
#include <stdint.h>

// Prevent the compiler from replacing the loops by calls to these functions
#define LIBC_ATTR __attribute__((used, optimize("no-tree-loop-distribute-patterns")))

LIBC_ATTR void* memcpy(void* dst, const void* src, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  const uint8_t* s = (const uint8_t*)src;
  while(n--) *d++ = *s++;
  return dst;
}

LIBC_ATTR void* memset(void* dst, int c, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  while(n--) *d++ = (uint8_t)c;
  return dst;
}

LIBC_ATTR int memcmp(const void* a, const void* b, size_t n) {
  const uint8_t* p = (const uint8_t*)a;
  const uint8_t* q = (const uint8_t*)b;
  while(n--) {
    if(*p != *q) return (int)*p - (int)*q;
    p++; q++;
  }
  return 0;
}
