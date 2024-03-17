# Programming the MCU using the factory built-in serial bootloader
## Description
With this tool, some entry-level STM32 microcontrollers can be flashed via a simple USB-to-serial converter by utilizing the factory built-in UART bootloader. It currently supports the following devices:
- STM32C011/031
- STM32F03xx4/6
- STM32G03x/04x
- STM32L01x/02x

## Preparations
In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PySerial](https://github.com/pyserial/pyserial) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyserial
```

If necessary, a driver for the USB-to-serial converter used must be installed.

## Connection
Connect your USB-to-serial converter to your STM32 MCU as follows:

```
USB2SERIAL      STM32C011/031
+--------+      +------------+
|     RXD| <--- |PA9  (PA11) |
|     TXD| ---> |PA10 (PA12) |
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+

USB2SERIAL      STM32F03xx4/6
+--------+      +------------+
|     RXD| <--- |PA9  or PA14|
|     TXD| ---> |PA10 or PA15|
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+

USB2SERIAL      STM32G03x/04x
+--------+      +------------+
|     RXD| <--- |PA2 or PA9  |
|     TXD| ---> |PA3 or PA10 |
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+

USB2SERIAL      STM32L01x/02x
+--------+      +------------+
|     RXD| <--- |PA2 or PA9  |
|     TXD| ---> |PA3 or PA10 |
|     3V3| ---> |VDD (3V3)   |
|     GND| ---> |GND         |
+--------+      +------------+
```

## Entering Bootloader Mode
Set your MCU to boot mode by using ONE of the following method:
- Disconnect your board from all power supplies, pull BOOT0 pin to VCC (or press and hold the BOOT button if your board has one), then connect the board to your USB port. The BOOT button can be released now.
- Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin to VCC, then pull nRST shortly to GND (or press and hold the BOOT button, then press and release the RESET button and then release the BOOT button, if your board has them).

On STM32C and STM32G microcontrollers, the BOOT0 pin is initially disabled. When the chip is brand new or the main flash memory is erased, this isn't an issue as the embedded bootloader automatically kicks in. By using the stm32isp tool, the BOOT0 pin will be activated for subsequent use. However, if the chip has been previously programmed using a different software tool, the bootloader might not be accessible through the BOOT0 pin anymore. In such cases, the nBOOT_SEL bit in the User Option Bytes must be cleared (set to 0) using an SWD programmer like ST-Link and the appropriate software.

## Usage
```
Usage: stm32isp.py [-h] [-u] [-l] [-e] [-f FLASH]

Optional arguments:
  -h, --help                show this help message and exit
  -u, --unlock              unlock chip (remove read protection)
  -l, --lock                lock chip (set read protection)
  -e, --erase               perform chip erase (implied with -f)
  -f FLASH, --flash FLASH   write BIN file to flash and verify

Example:
python3 stm32isp.py -f firmware.bin
```
