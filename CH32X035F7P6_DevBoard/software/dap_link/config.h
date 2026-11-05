// ===================================================================================
// User Configurations for CMSIS-DAP v2 Debug Probe (CH32X035)
// ===================================================================================
//
// Pin assignments, interface options and USB descriptors of the debug probe.
//
// Default wiring on the CH32X035F7P6 development board:
//
//   Probe  | SWD mode | JTAG mode | Function
//   -------+----------+-----------+------------------------------------------------
//   PA5    | SWCLK    | TCK       | clock output
//   PA7    | SWDIO    | TMS       | data input/output (SWD), mode select (JTAG)
//   PA4    | -        | TDI       | test data output to target
//   PA6    | -        | TDO       | test data input from target
//   PA0    | nRESET   | nRESET    | target reset (open-drain, low active)
//   PA1    | -        | nTRST     | JTAG test reset (open-drain, low active)
//   PC19   | SWO      | (TDO/SWO) | SWO trace input (UART/NRZ mode)
//   PA2    | TX       | TX        | UART (virtual COM port) output to target RX
//   PA3    | RX       | RX        | UART (virtual COM port) input from target TX
//   PB1    | LED      | LED       | status LED (on-board LED)
//   GND    | GND      | GND       | ground
//   3V3    | VTref    | VTref     | target supply (3.3V only)

#pragma once

// ===================================================================================
// Debug Interface Pins (SWD/JTAG)
// ===================================================================================
// Any GPIO pins can be used, pins on port A (PA0..PA7) give the fastest switching.
#define PIN_SWCLK_TCK       PA5       // SWCLK (SWD) / TCK (JTAG)
#define PIN_SWDIO_TMS       PA7       // SWDIO (SWD) / TMS (JTAG)
#define PIN_TDI             PA4       // TDI (JTAG only)
#define PIN_TDO             PA6       // TDO (JTAG only)
#define PIN_nRESET          PA0       // target reset (comment out if not used)
#define PIN_nTRST           PA1       // JTAG TRST (comment out if not used)

// ===================================================================================
// Status LEDs (optional, comment out if not used)
// ===================================================================================
// PIN_LED_CONNECTED: on = debugger connected, blinking = USB not configured
// PIN_LED_RUNNING:   on = debugger has put the target into running state
//                    (if not defined, the running state is shown by PIN_LED_CONNECTED
//                    flickering during data transfers)
#define PIN_LED_CONNECTED   PB1       // on-board LED
//#define PIN_LED_RUNNING   PB12      // second LED (optional)
#define LED_ACTIVE_LEVEL    0         // pin level that switches the LEDs ON

// ===================================================================================
// USART Pin Mapping Table of the CH32X035 (pins on the F7P6 board marked with *)
// ===================================================================================
// USARTx | REMAP | TX     | RX     | Notes
// -------+-------+--------+--------+------------------------------------------------
//    1   |   3   | PA7*   | PB2    |
//    2   |   0   | PA2*   | PA3*   | default for UART
//    2   |   3   | PC0    | PC1*   |
//    3   |   1   | PC18*  | PC19*  | default for SWO (RX only), disables the SDI
//        |       |        |        | debug interface of the CH32X035 itself
//    4   |   0   | PB0    | PB1*   | conflicts with on-board LED
//    4   |   1   | PA5*   | PA9    |
//    4   |   4   | PB13   | PC19*  | disables the SDI debug interface of the CH32X035
// UART and SWO must use different USARTs.

// ===================================================================================
// UART (USB Virtual COM Port and DAP_UART Commands)
// ===================================================================================
#define DAP_UART_ENABLE     1         // 1: UART enabled, 0: disabled
#define UART_USART          2         // USART number (1..4)
#define UART_REMAP          0         // USART remap value (see table above)
#define PIN_UART_TX         PA2       // TX pin (must match USART and REMAP)
#define PIN_UART_RX         PA3       // RX pin (must match USART and REMAP)
#define UART_COM_PORT       1         // 1: provide USB virtual COM port (CDC-ACM)
#define UART_RX_BUF_SIZE    1024      // UART receive buffer size (2^n)
#define UART_TX_BUF_SIZE    1024      // UART transmit buffer size (2^n)

// ===================================================================================
// SWO Trace (UART/NRZ Mode)
// ===================================================================================
#define DAP_SWO_ENABLE      1         // 1: SWO enabled, 0: disabled
#define SWO_USART           3         // USART number (1..4)
#define SWO_REMAP           1         // USART remap value (see table above)
#define PIN_SWO             PC19      // RX pin (must match USART and REMAP)
#define SWO_BUF_SIZE        4096      // SWO trace buffer size (2^n)

// ===================================================================================
// CMSIS-DAP Settings
// ===================================================================================
// DAP packet size: must not be a multiple of the USB packet size (64) for
// compatibility with all host tools, 2 mod 4 values (e.g. 510) are recommended.
#define DAP_PACKET_SIZE     510       // DAP packet size in bytes
#define DAP_PACKET_COUNT    4         // number of buffered packets (2^n)
#define DAP_JTAG_DEV_CNT    8         // max number of JTAG devices on scan chain
#define DAP_DEFAULT_PORT    1         // default port mode: 1 = SWD, 2 = JTAG
#define DAP_DEFAULT_CLOCK   1000000   // default SWD/JTAG clock frequency in Hz

// Product firmware version reported via DAP_Info
#define DAP_PRODUCT_FW_VER  "1.0"

// ===================================================================================
// USB Descriptors
// ===================================================================================
// USB device descriptor. The host tools identify CMSIS-DAP probes by the string
// "CMSIS-DAP" in the product/interface string, not by VID/PID.
// 1209:0001 is the pid.codes test PID (for private use only). Alternatives:
// 0D28:0204 (ARM DAPLink), 1A86:8012 (WCH-LinkE in CMSIS-DAP mode).
#define USB_VENDOR_ID       0x1A86    // VID
#define USB_PRODUCT_ID      0x8012    // PID
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA (incl. target supply)

// MCU supply voltage (board with 3.3V regulator: 0)
#define USB_VDD             0         // 0: 3.3V, 1: 5V

// USB descriptor strings (product and DAP interface string must contain "CMSIS-DAP")
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH32X035 CMSIS-DAP"
#define SERIAL_STR          "CH32X035DAP"
#define DAP_INTERF_STR      "CH32X035 CMSIS-DAP"
#define CDC_INTERF_STR      "CH32X035 CMSIS-DAP UART"

// Serial number: 1: generated from the 96-bit unique ID of the CH32X035
//                0: SERIAL_STR is used
#define USB_SERIAL_UID      1

// Microsoft OS 2.0 descriptors (1: WinUSB driver is installed automatically on
// Windows for the CMSIS-DAP v2 interface)
#define USB_WINUSB          1
#define USB_MS_VENDOR_CODE  0x20      // vendor request code for MS OS 2.0 descriptor
