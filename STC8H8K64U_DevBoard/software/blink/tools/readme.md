# Programming the MCU using the factory built-in USB bootloader
## Description
With this tool, STC8H8KxxU microcontrollers can be flashed via their USB interface by utilizing the factory built-in embedded bootloader.

## Preparations
In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [pyhidapi](https://pypi.org/project/hid/) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install hid
```

By default Linux will not expose enough permission to access the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="34bf", ATTR{idProduct}=="1001", MODE="666"' | sudo tee /etc/udev/rules.d/99-STC-ISP.rules
sudo udevadm control --reload-rules
```

## Usage
The bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT button can be released and new firmware can be uploaded via USB. If there is no BOOT button on the board, you have to short pin P3.2 to ground while connecting.

```
Usage: stc8usb.py [-h] [-t TRIM] [-e] [-f FLASH]

Optional arguments:
  -h,       --help          show this help message and exit
  -t TRIM,  --trim TRIM     set MCU system frequency
  -e,       --erase         perform chip erase (implied with -f)
  -f FLASH, --flash FLASH   write BIN file to flash

Example:
python3 stc8usb.py -t 24000000 -f firmware.bin
```
