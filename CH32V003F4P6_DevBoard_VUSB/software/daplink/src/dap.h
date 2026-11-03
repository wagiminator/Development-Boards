// ===================================================================================
// CMSIS-DAP Functions (SWD only) for CH32V003                                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DAP_init()               init DAP (all SWD pins to high impedance)
// DAP_process(req, res)    process one DAP request, returns length of response
// DAP_abort                set to 1 to abort the current transfer (DAP_TransferAbort)
//
// Supported commands: DAP_Info, DAP_HostStatus, DAP_Connect, DAP_Disconnect,
// DAP_TransferConfigure, DAP_Transfer, DAP_TransferBlock, DAP_TransferAbort,
// DAP_WriteABORT, DAP_Delay, DAP_ResetTarget, DAP_SWJ_Pins, DAP_SWJ_Clock,
// DAP_SWJ_Sequence, DAP_SWD_Configure, DAP_SWD_Sequence.
//
// The implementation of the SWD protocol and the DAP commands is based on
// free-dap by Alex Taradov (BSD-3-Clause, see dap.c) and the CMSIS-DAP reference
// implementation by ARM (https://github.com/ARM-software/CMSIS-DAP).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "config.h"

// DAP packet definitions
#define DAP_PACKET_SIZE           64    // must match HID report size
#define DAP_PACKET_COUNT          1     // number of buffered packets
#define DAP_FW_VERSION            "1.3.0"

// DAP command IDs
#define ID_DAP_Info               0x00
#define ID_DAP_HostStatus         0x01
#define ID_DAP_Connect            0x02
#define ID_DAP_Disconnect         0x03
#define ID_DAP_TransferConfigure  0x04
#define ID_DAP_Transfer           0x05
#define ID_DAP_TransferBlock      0x06
#define ID_DAP_TransferAbort      0x07
#define ID_DAP_WriteABORT         0x08
#define ID_DAP_Delay              0x09
#define ID_DAP_ResetTarget        0x0A
#define ID_DAP_SWJ_Pins           0x10
#define ID_DAP_SWJ_Clock          0x11
#define ID_DAP_SWJ_Sequence       0x12
#define ID_DAP_SWD_Configure      0x13
#define ID_DAP_SWD_Sequence       0x1D
#define ID_DAP_Invalid            0xFF

// Global variables
extern volatile uint8_t DAP_abort;      // abort current transfer

// Functions
void DAP_init(void);                                // init DAP
uint8_t DAP_process(const uint8_t *req, uint8_t *res); // process request
