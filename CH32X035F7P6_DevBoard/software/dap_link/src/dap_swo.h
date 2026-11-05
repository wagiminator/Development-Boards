// ===================================================================================
// CMSIS-DAP SWO Trace (UART/NRZ Mode) for CH32X035                           * v1.0 *
// ===================================================================================
//
// SWO_init()           setup SWO USART and pin (call once at startup)
// SWO_task()           send trace data via SWO endpoint (call in main loop)
//
// The DAP_SWO_xxx command functions are declared in cmsis_dap/DAP.h.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SWO_STREAM_PACKET   64        // max bytes per streaming transfer (USB packet)

void SWO_init(void);
void SWO_task(void);

#ifdef __cplusplus
}
#endif
