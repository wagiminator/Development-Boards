// ===================================================================================
// 8-Channel Logic Analyzer (sigrok-pico Protocol) User Configurations
// ===================================================================================
//
// Description:
// ------------
// 8-channel logic analyzer for the CH32X033 development board. It speaks the serial
// protocol of the sigrok-pico firmware via USB CDC, so it works with PulseView and
// sigrok-cli (driver "raspberrypi-pico") and with the Signal Analyzer web app.
//
// Channels and pins:
// ------------------
// D0..D7   PA0..PA7    logic inputs (2.4V = HIGH at 5V supply, so 3.3V and 5V logic)
// PB1                  built-in LED, lights up while sampling
// PC3                  test signal output (square wave with TEST_FREQ, 0 = off)
//
// Sample rates:
// -------------
// The sample clock is the system clock (48 MHz) divided by an integer, so possible
// rates are 48 MHz / n, e.g. 12M, 8M, 6M, 4M, 3M, 2M, 1M .. 5k. Other rates (like 5M
// or 10M) are refused.
// - Captures of up to BUF_SIZE samples without trigger are first stored in RAM and
//   then sent. This works up to SAMPLE_RATE_MAX.
// - Longer captures and captures with trigger (continuous mode, triggering is done
//   by the host) are streamed via USB while sampling. How fast this can go depends on
//   how often the signals change: around 1 MS/s for busy signals, more for quiet
//   ones. If the USB cannot keep up, the capture is aborted ("!").

#pragma once

// Pin definitions
#define PIN_LED             PB1       // pin connected to LED (active low)
#define PIN_TEST            PC3       // test signal output (TIM1 CH4, fixed)

// Logic inputs (always PA0..PA7)
#define INPUT_PULLDOWN      1         // 1: weak pull-down on inputs, 0: floating

// Test signal on PC3 (connect it to an input to check the analyzer)
#define TEST_FREQ           1000      // frequency in Hz (0 = off)

// Sampling
#define BUF_SIZE            16384     // sample buffer in RAM (bytes = samples)
#define SAMPLE_RATE_MAX     8000000   // highest sample rate accepted (Hz)
#define SAMPLE_RATE_MIN     1000      // lowest sample rate accepted (Hz)

// MCU supply voltage
#define USB_VDD             1         // 0: 3.3V, 1: 5V

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA

// USB descriptor strings
#define MANUF_STR           "wagiminator"
#define PROD_STR            "CH32X033 Logic Analyzer"
#define SERIAL_STR          "CH32X035SRPICO"
#define INTERF_STR          "CDC-Serial"
