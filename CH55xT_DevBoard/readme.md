# CH55xT Development Board
Development board for the CH552T, CH554T and CH559T microcontrollers with native USB 2.0 and Arduino IDE support.

![CH55xT_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xT_DevBoard/documentation/CH55xT_DevBoard_pic1.jpg)

# Pinout
![CH55xT_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xT_DevBoard/documentation/CH55xT_DevBoard_pinout.png)

Note: The CH559T partially has a different pinout. Refer to the [datasheet](http://www.wch-ic.com/downloads/CH559DS1_PDF.html) for more information.

# Features of the CH55xT
The CH55x is an 8-bit enhanced E8051 core MCU compatible with the MCS51 instruction set. 79% of its instructions are single-byte single-cycle instructions, and the average instruction speed is 8 - 15 times faster than that of the standard MCS51. The special xRAM supports direct memory access (DMA), data fast copy command, and double DPTR pointers. The CH55x has a built-in USB controller and USB transceiver, it supports USB-Host mode (except CH552) and USB-Device mode, USB Type-C master-slave detection (except CH559), USB 2.0 full-speed (12Mbps) and low-speed (1.5Mbps) traffic, data packets of up to 64 bytes, and DMA. It has a factory built-in bootloader so firmware can be uploaded directly via USB without the need for an additional programming device.

 Specifications |  CH552T |  CH554T |  CH559T 
:-------------- | :------ | :------ | :------ 
Program ROM  | 14 KB | 14 KB | 60 KB
Bootloader | 2 KB | 2 KB | 3 KB
Data Flash | 128 bytes | 128 bytes | 1 KB
iRAM | 256 bytes | 256 bytes | 256 bytes 
xRAM | 1024 bytes | 1024 bytes | 6144 bytes
GPIO Pins | 16 (14 usable) | 16 (14 usable) | 16 (14 usable)
ADC Channels | 4x 8-bit | 4x 8-bit | 8x 10/11-bit
PWM Channels | 2x 8-bit | 2x 8-bit | 2x 8-bit, 1x 16-bit
Timer/Counter | 3 | 3 | 4
Touch Key Channels | 6 | 6 | 0
USB Modes | Device | Host/Device | Host/Device
USB Type-C | yes | yes | no
Interfaces | 2x UART, 1x SPI | | 2x UART, 1x SPI | 2x UART, 2x SPI 
Internal Clock | max 24 MHz | max 24 MHz | max 56 MHz 

# Built-in USB Bootloader
## Installing Drivers for the Bootloader
### Linux
You do not need to install a driver. However, by default Linux will not expose enough permission to upload your code with the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
sudo service udev restart
```

### Windows
On Windows you will need the Zadig tool (https://zadig.akeo.ie/) to install the correct driver. Click "Options" and "List All Devices" to select the USB module, then install the libusb-win32 driver.

## Entering Bootloader Mode
A brand new chip starts automatically in bootloader mode as soon as it is connected to the PC via USB. Once firmware has been uploaded, the bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts again in bootloader mode, the BOOT button can be released and new firmware can be uploaded.

# Installing Toolchain for CH55x
If you do not want to use the Arduino IDE, you can install the [CH55x Software Development Kit for the SDCC compiler](https://github.com/Blinkinlabs/ch554_sdcc). Follow the instructions on the website. This SDK does not support the CH559 at the moment!

In order for some programming tools to work, Python3 must be installed on your system. Most Linux distributions already include this. Windows users can follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition pyusb must be installed. On Linux this can be done by running the following commands:

```
sudo apt install python3-pip
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
- [EasyEDA Design Files](https://oshwlab.com/wagiminator/ch55xt-development-board)
- [CH55x SDK for SDCC](https://github.com/Blinkinlabs/ch554_sdcc)
- [ch55xduino](https://github.com/DeqingSun/ch55xduino)
- [CH552 Datasheet](http://www.wch-ic.com/downloads/CH552DS1_PDF.html)
- [CH554 Datasheet](http://www.wch-ic.com/downloads/CH554DS1_PDF.html)
- [CH559 Datasheet](http://www.wch-ic.com/downloads/CH559DS1_PDF.html)

![CH55xT_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xT_DevBoard/documentation/CH55xT_DevBoard_pic2.jpg)
![CH55xT_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xT_DevBoard/documentation/CH55xT_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
