# Programming the MCU using the factory built-in serial bootloader
## Description
With this tool, STC8G/8H microcontrollers can be flashed via a simple USB-to-serial converter by utilizing the factory built-in embedded bootloader.

## Preparations
In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PySerial](https://github.com/pyserial/pyserial) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyserial
```

If necessary, a driver for the USB-to-serial converter used must be installed.

## Connection
Connect your USB-to-serial converter to your MCU as follows:

```
USB2SERIAL         STC8G/8H
+--------+         +------+
|     VCC| --/ --> |VCC   |    interruptible (for power cycle)
|     RXD| --|R|-- |P3.1  |    resistor (100R - 1000R)
|     TXD| --|<|-- |P3.0  |    diode (e.g. 1N5819)
|     GND| ------- |GND   |    common ground
+--------+         +------+
```

## Usage
- Run stc8isp.py (see below for arguments).
- Perform a power cycle of your MCU (reconnect to power) when prompted.

```
Usage: stc8isp.py [-h] [-p PORT] [-t TRIM] [-e] [-f FLASH]

Optional arguments:
  -h,       --help          show this help message and exit
  -p PORT,  --port PORT     set COM port (default: /dev/ttyUSB0)
  -t TRIM,  --trim TRIM     trim IRC to frequency in Hz (128000 - 36000000)
  -e,       --erase         perform chip erase (implied with -f)
  -f FLASH, --flash FLASH   write BIN file to flash

Example:
python3 stc8isp.py -p /dev/ttyUSB0 -t 24000000 -f firmware.bin
```
