# Programming the MCU using the factory built-in serial bootloader
## Description
With this tool, PUYA microcontrollers of the series PY32F0xx (and maybe other PY32) can be flashed via a simple USB-to-serial converter by utilizing the factory built-in embedded bootloader.

## Preparations
In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PySerial](https://github.com/pyserial/pyserial) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyserial
```

If necessary, a driver for the USB-to-serial converter used must be installed.

## Connection
Connect your USB-to-serial converter to your PY32F0xx MCU as follows:

```
USB2SERIAL            PY32F0xx
+--------+      +-------------------+
|     RXD| <--- |PA2 or PA9  or PA14|
|     TXD| ---> |PA3 or PA10 or PA15|
|     VDD| ---> |VDD                |
|     GND| ---> |GND                |
+--------+      +-------------------+
```

## Entering Bootloader Mode
Set your MCU to boot mode by using ONE of the following methods:
- Disconnect your USB-to-serial converter, pull BOOT0 pin (PF4) to VCC (or press and hold the BOOT button, if your board has one), then connect the converter to your USB port. BOOT0 pin (or BOOT button) can be released now.
- Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin (PF4) to VCC, then pull nRST (PF2) shortly to GND (or press and hold the BOOT button, then press and release the RESET button and then release the BOOT button, if your board has them).

## Usage
```
Usage: puyaisp.py [-h] [-u] [-l] [-e] [-o] [-G] [-R] [-f FLASH]

Optional arguments:
  -h, --help                show this help message and exit
  -u, --unlock              unlock chip (remove read protection)
  -l, --lock                lock chip (set read protection)
  -e, --erase               perform chip erase (implied with -f)
  -o, --rstoption           reset option bytes
  -G, --nrstgpio            make nRST pin a GPIO pin
  -R, --nrstreset           make nRST pin a RESET pin
  -f FLASH, --flash FLASH   write BIN file to flash and verify

Example:
python3 puyaisp.py -f firmware.bin
```
