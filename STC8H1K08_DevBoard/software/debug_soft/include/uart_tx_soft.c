// ===================================================================================
// Software-UART (TX only) for STC8H Microcontrollers                         * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include	"uart_tx_soft.h"

#define _TXPIN PIN_asm(PIN_TXD)

// Bit-delay according to BAUD rate
void SUART_delay(void) {
  __asm
    mov dptr, #(((F_CPU / SUART_BAUD) / 3) - 4)
    xch  a, dpl
    jz   01$
    inc  dph
    01$:
    xch  a, dpl
    02$:
    djnz dpl, 02$
    djnz dph, 02$
  __endasm;
}

// Write single character via SUART (8N1)
void SUART_write(uint8_t data) {
  data;
  __asm
    mov   a, dpl          ; data byte -> accu
    clr   _EA             ; disable interrupt for precise timing
    clr   _TXPIN          ; start bit: TX-pin low
    lcall _SUART_delay    ; delay
    mov   r7, #8          ; 8 bits, LSB first
    03$:
    rrc   a               ; shift data bit right into carry
    mov   _TXPIN, c       ; set TX-pin according to carry
    lcall _SUART_delay    ; delay
    djnz  r7, 03$         ; repeat for all 8 bits
    setb  _TXPIN          ; stop bit: TX-pin high
    setb  _EA             ; re-enable interrupts
    lcall _SUART_delay    ; delay
  __endasm;
}
