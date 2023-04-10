# CH55xG Development Board
Development board for the CH551G, CH552G and CH554G microcontrollers with native USB 2.0 and Arduino IDE support.

![CH55xG_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xG_DevBoard/documentation/CH55xG_DevBoard_pic1.jpg)

# Pinout
![CH55xG_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xG_DevBoard/documentation/CH55xG_DevBoard_pinout.png)

# The CH55xG Family of 8-bit USB Microcontrollers
## Overview
The CH55x is an 8-bit enhanced E8051 core MCU compatible with the MCS51 instruction set. 79% of its instructions are single-byte single-cycle instructions, and the average instruction speed is 8 - 15 times faster than that of the standard MCS51. The special xRAM supports direct memory access (DMA), data fast copy command, and double DPTR pointers. The CH55x has a built-in USB controller and USB transceiver, it supports USB-Host mode (CH554 only) and USB-Device mode (all), USB Type-C master-slave detection, USB 2.0 full-speed (12Mbps) and low-speed (1.5Mbps) traffic, data packets of up to 64 bytes, and DMA. It has a factory built-in bootloader so firmware can be uploaded directly via USB without the need for an additional programming device.

## Block Diagram
![CH55xG_DevBoard_block.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xG_DevBoard/documentation/CH55xG_DevBoard_block.jpg)

## Features

 Specifications |  CH551G |  CH552G |  CH554G 
:-------------- | :------ | :------ | :------ 
Program ROM  | 10 KB | 14 KB | 14 KB
Bootloader | 2 KB | 2 KB | 2 KB
Data Flash | 128 bytes | 128 bytes | 128 bytes
iRAM | 256 bytes | 256 bytes | 256 bytes 
xRAM | 512 bytes | 1024 bytes | 1024 bytes
GPIO Pins | 12 (10 usable) | 12 (10 usable) | 12 (10 usable)
ADC Channels | 0 | 4x 8-bit | 4x 8-bit
PWM Channels | 2x 8-bit | 2x 8-bit | 2x 8-bit
Timer/Counter | 3 | 3 | 3
Touch Key Channels | 4 | 5 | 5
USB Modes | Device | Device | Host/Device
USB Type-C | no | yes | yes
Interfaces | 1x UART, SPI | 2x UART, SPI | 2x UART, SPI
Internal Clock | 24/16/12/6/3/0.75 MHz | 24/16/12/6/3/0.75 MHz | 24/16/12/6/3/0.75 MHz 

# Built-in USB Bootloader
## Installing Drivers for the Bootloader
On Linux you do not need to install a driver. However, by default Linux will not expose enough permission to upload your code with the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
sudo service udev restart
```

For Windows, you need the [CH372 driver](http://www.wch-ic.com/downloads/CH372DRV_EXE.html). Alternatively, you can also use the [Zadig Tool](https://zadig.akeo.ie/) to install the correct driver. Here, click "Options" -> "List All Devices" and select the USB module. Then install the libusb-win32 driver. To do this, the board must be connected and the CH55x must be in bootloader mode.

## Entering Bootloader Mode
A brand new chip starts automatically in bootloader mode as soon as it is connected to the PC via USB. Once firmware has been uploaded, the bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts again in bootloader mode, the BOOT button can be released and new firmware can be uploaded within the next couple of seconds.

# Installing Toolchain for CH55x
If you do not want to use the Arduino IDE, you can install the [CH55x Software Development Kit for the SDCC compiler](https://github.com/Blinkinlabs/ch554_sdcc). Follow the instructions on the website.

In order for some programming tools to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [pyusb](https://github.com/pyusb/pyusb) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install build-essential sdcc python3 python3-pip
sudo pip install pyusb
```

# Preparing Arduino IDE
- [Install ch55xduino](https://github.com/DeqingSun/ch55xduino).
- Open the Arduino IDE.
- Go to "Tools" -> "Board" -> "CH55x Boards" and select your chip.
- Go to "Tools" and set the desired board options.
- Go to "File" -> "Examples" -> "Examples for CH55x Board" and select a Sketch of your choice.
- If your board is never used with ch55xduino before, you need to make the ch55x chip enter the bootloader mode. Hold the BOOT button pressed while connecting the board to the USB port of your PC. You do not need to do this if you have installed a blank new chip on the board.
- Now you can upload your sketch.

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator/ch55xg-development-board)
- [CH55x SDK for SDCC](https://github.com/Blinkinlabs/ch554_sdcc)
- [ch55xduino](https://github.com/DeqingSun/ch55xduino)
- [CH551/552 Datasheet](http://www.wch-ic.com/downloads/CH552DS1_PDF.html)
- [CH554 Datasheet](http://www.wch-ic.com/downloads/CH554DS1_PDF.html)

![CH55xG_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xG_DevBoard/documentation/CH55xG_DevBoard_pic3.jpg)
![CH55xG_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xG_DevBoard/documentation/CH55xG_DevBoard_pic2.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
