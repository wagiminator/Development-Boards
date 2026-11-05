// ===================================================================================
// CMSIS Compiler Macros for GCC on RISC-V (replacement of ARM cmsis_compiler.h)
// ===================================================================================
// Minimal set of the CMSIS compiler macros used by the CMSIS-DAP sources.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifndef __ASM
#define __ASM                   __asm
#endif
#ifndef __STATIC_INLINE
#define __STATIC_INLINE         static inline
#endif
#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE    __attribute__((always_inline)) static inline
#endif
#ifndef __WEAK
#define __WEAK                  __attribute__((weak))
#endif
#ifndef __NO_RETURN
#define __NO_RETURN             __attribute__((__noreturn__))
#endif
#ifndef __NOP
#define __NOP()                 __asm volatile ("nop")
#endif
