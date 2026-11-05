// ===================================================================================
// CMSIS-DAP UART Communication Port for CH32X035                             * v1.0 *
// ===================================================================================
//
// UART_init()          setup UART and activate USB COM port (call once at startup)
// UART_port            USART instance shared by DAP_UART commands and USB COM port
//
// The DAP_UART_xxx command functions are declared in cmsis_dap/DAP.h.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "usart_dma.h"

extern USARTX_t UART_port;
void UART_init(void);

#ifdef __cplusplus
}
#endif
