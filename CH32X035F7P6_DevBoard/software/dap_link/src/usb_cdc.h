// ===================================================================================
// USB CDC-ACM Virtual COM Port for CMSIS-DAP Debug Probe                     * v1.0 *
// ===================================================================================
//
// Bridge between the USB CDC-ACM interface and the UART of the debug probe. The
// line coding (baudrate, data bits, parity, stop bits) set by the host is applied
// to the UART. The COM port is active when the UART transport is set to the USB COM
// port (default), see DAP_UART_Transport.
//
// CDC_task()           transfer data between USB and UART (call in main loop)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Line coding
typedef struct __attribute__((packed)) {
  uint32_t baudrate;                      // baud rate
  uint8_t  stopbits;                      // 0: 1 stop bit, 1: 1.5 stop bits, 2: 2 stop bits
  uint8_t  parity;                        // 0: none, 1: odd, 2: even, 3: mark, 4: space
  uint8_t  databits;                      // 5, 6, 7, 8 or 16
} CDC_LINE_CODING_TYPE, *PCDC_LINE_CODING_TYPE;

extern CDC_LINE_CODING_TYPE CDC_lineCoding;
extern volatile uint8_t CDC_controlLineState;

void    CDC_task(void);

// USB handler functions
void    CDC_EP_init(void);
uint8_t CDC_control(void);
void    CDC_EP0_OUT(void);
void    CDC_EP5_IN(void);
void    CDC_EP5_OUT(void);

#ifdef __cplusplus
}
#endif
