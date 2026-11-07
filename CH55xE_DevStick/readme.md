# CH55xE Development Stick
Development stick for the CH552E and CH554E 8-bit USB microcontrollers with a built-in LED on a PWM-capable pin, a WS2812 programmable LED (NeoPixel), a physical and a touch key, native USB 2.0, and Arduino IDE support. Explore the features of this cheap microcontroller without hooking up any wires!

![CH55xE_DevStick_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xE_DevStick/documentation/CH55xE_DevStick_pic1.jpg)

# Pinout
![CH55xE_DevStick_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xE_DevStick/documentation/CH55xE_DevStick_pinout.png)

Pin|Connected to
---|------------------------
P1.4|WS2812 NeoPixel
P1.5|LED (PWM), active low
P1.6|Touch key (bottom side)
P1.7|Physical ACT-button
P3.6|USB D+
P3.7|USB D-
RST|Physical RESET-button

# The CH55xE Family of 8-bit USB Microcontrollers
## Overview
The CH55x is a low-cost, 8-bit enhanced E8051 core microcontroller compatible with the MCS51 instruction set. 79% of its instructions are single-byte single-cycle instructions, and the average instruction speed is 8 - 15 times faster than that of the standard MCS51. The special xRAM supports direct memory access (DMA), data fast copy command, and double DPTR pointers. The CH55x has a built-in USB 2.0 controller and USB transceiver, it supports USB-Host mode (CH554 only) and USB-Device mode, USB Type-C master-slave detection, full-speed (12Mbps) and low-speed (1.5Mbps) traffic, data packets of up to 64 bytes, and DMA. It has a factory built-in bootloader so firmware can be uploaded directly via USB without the need for an additional programming device.

## Block Diagram
![CH55xG_DevBoard_block.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xG_DevBoard/documentation/CH55xG_DevBoard_block.jpg)

## Features

 Specifications |  CH552E |  CH554E 
:-------------- | :------ | :------ 
Program ROM  | 14 KB | 14 KB
Bootloader | 2 KB | 2 KB
Data Flash | 128 bytes | 128 bytes
iRAM | 256 bytes | 256 bytes 
xRAM | 1024 bytes | 1024 bytes
GPIO Pins | 4 + 2*USB | 4 + 2*USB
ADC Channels | 4x 8-bit | 4x 8-bit
PWM Channels | 2x 8-bit | 2x 8-bit
Timer/Counter | 3 | 3
Touch Key Channels | 4 | 4
USB modes | Device | Host/Device
USB Type-C | yes | yes
Interfaces | 2x UART, SPI | 2x UART, SPI
Intern. Clock | 24/16/12/6/3/0.75 MHz | 24/16/12/6/3/0.75 MHz

# Programming and Debugging
The CH55x has a factory built-in bootloader, so firmware can be uploaded directly via USB without the need for an additional programming device.

## Built-in USB Bootloader
### Installing Drivers for the Bootloader
On Linux you do not need to install a driver. However, by default Linux will not expose enough permission to upload your code with the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

On Windows, you need the [CH372 driver](https://www.wch-ic.com/downloads/CH372DRV_EXE.html). Alternatively, you can also use the [Zadig](https://zadig.akeo.ie/) tool to install the correct driver. Here, click "Options" -> "List All Devices" and select the USB module. Then install the libusb-win32 driver. To do this, the board must be connected and the CH55x must be in bootloader mode.

### Entering Bootloader Mode
A brand new chip starts automatically in bootloader mode as soon as it is connected to the PC via USB. Once firmware has been uploaded, the bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT button can be released and new firmware can be uploaded via USB within the next couple of seconds.

### Uploading Firmware with chprog
The open-source platform-independent Python tool [chprog](https://pypi.org/project/chprog/) from the [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools) is used by the makefiles of the example software. In order for this tool to work, [Python3](https://www.python.org/downloads/) with [pip](https://pip.pypa.io/en/stable/getting-started/) must be installed on your system. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
pip install chprog
```

Usage example:

```
chprog firmware.bin
```

Alternatively, you can use the [chprog web app](https://wagiminator.github.io/apps/chprog/) directly in your web browser (Chrome, Edge or Opera on a desktop computer) without installing any software.

### Alternative Software Tools
- [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) - official closed-source tool by WCH (Windows only)
- [isp55e0](https://github.com/frank-zago/isp55e0)
- [wchisp](https://github.com/ch32-rs/wchisp)

# Software Development Tools
## Open-Source SDCC Toolchain
For compiling install the [SDCC](https://sdcc.sourceforge.net/) compiler. On Linux (Debian-based), this can be done with the following command:

```
sudo apt install build-essential sdcc
```

The [example software](#example-software) is based on the [MCU Templates](https://github.com/wagiminator/MCU-Templates/tree/main/CH552), a lightweight starting point for bare-metal firmware development in C without any IDE. Each example folder contains a makefile. Go into the folder of an example and use one of the following commands:

```
make all       compile and build firmware
make flash     compile and upload firmware to the MCU
make clean     remove all build files
make help      show all options
```

The [CH55x Software Development Kit](https://github.com/Blinkinlabs/ch554_sdcc) by Blinkinlabs is another option for SDCC.

## Arduino IDE Support
- [Install ch55xduino](https://github.com/DeqingSun/ch55xduino) (tested with version 0.0.16).
- Open the Arduino IDE.
- Go to **Tools -> Board -> CH55x Boards** and select your chip.
- Go to **Tools** and set the desired board options.
- Go to **File -> Examples -> Examples for CH55x Board** and select a sketch of your choice.
- If your board has never been used with ch55xduino before, you need to make the chip enter the bootloader mode (see above).
- Now you can upload your sketch.

# Example Software
The following examples are included. They use the [MCU Templates](https://github.com/wagiminator/MCU-Templates) and their libraries. The USB examples use shared VID/PID pairs for testing purposes only.

Some of the older examples (e.g. blink, bootkey, capsblock) are not yet based on the current MCU Templates. They contain their own copy of the flash tool in the *tools* folder, which is called via `python3` by the makefile. They can also be uploaded with the pip version of the tool. The examples with an *.ino* file can also be compiled and uploaded with the Arduino IDE (see the instructions in the *.ino* file).

| Example | Description |
|:-|:-|
| [blink](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/blink) | Blink the built-in LED |
| [bootkey](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/bootkey) | Jump to bootloader on ACT-button press. Built-in LED lights up when in boot mode. |
| [capsblock](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/capsblock) | Tired of accidentally hitting the CAPS-LOCK key? It doesn't have to be. CAPS bLOCK identifies itself as a USB HID keyboard and constantly monitors the state of CAPS LOCK. It immediately deactivates it if it has been activated. |
| [cdc_echo](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/cdc_echo) | Simple USB-CDC demo which echoes back all received messages. |
| [cdc_neo](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/cdc_neo) | Controls the color of the NeoPixel via USB CDC (virtual serial port). Send "r", "g", "b", "w" or "0" to set the color to red, green, blue, white or off, or a digit "1" to "9" to set a hue value. The current color is reported back. |
| [flashdump](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/flashdump) | Transfers content of data flash via USB-CDC on ACT-Button press. |
| [mediakey](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/mediakey) | The device identifies itself as a USB HID composite device (keyboard, mouse and multimedia keys). Pressing the ACT button sends PLAY/PAUSE, touching the touch key sends MUTE. The NeoPixel lights up while a key is pressed. |
| [mousewiggler](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/mousewiggler) | Mouse Wiggler is a tool that simulates movements of the mouse pointer so that no screen saver is activated and the PC or notebook is not put to sleep. |
| [neopixel](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/neopixel) | Simple demonstration of driving an addressable LED. |
| [neotouch](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/neotouch) | Control NeoPixel color by holding the touch key. |
| [pwm_fade](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/pwm_fade) | Fade the built-in LED using PWM |
| [rubberducky](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/rubberducky) | Identifies itself as a USB HID keyboard and types a message when the ACT key is pressed. It can be used to control the PC via keyboard shortcuts. The built-in LED shows the status of CAPS LOCK (just for demonstration). |
| [touchkey](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/touchkey) | Toggle built-in LED by pressing the touch key. |
| [touchraw](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/touchraw) | Continuously transmits touchkey raw sample values via USB-CDC. |
| [watchdog](https://github.com/wagiminator/Development-Boards/tree/main/CH55xE_DevStick/software/watchdog) | Flashes the built-in LED by regular watchdog timer resets. Uses the RESET_KEEP register to retain LED status across reset. |

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator/ch55xe-development-stick)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [CH55x SDK for SDCC by Blinkinlabs](https://github.com/Blinkinlabs/ch554_sdcc)
- [ch55xduino](https://github.com/DeqingSun/ch55xduino)
- [CH552 Datasheet](https://www.wch-ic.com/downloads/CH552DS1_PDF.html)
- [CH554 Datasheet](https://www.wch-ic.com/downloads/CH554DS1_PDF.html)
- [WCH Official Store on AliExpress](https://wchofficialstore.aliexpress.com)
- [chprog Web App](https://wagiminator.github.io/apps/chprog/)

![CH55xE_DevStick_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xE_DevStick/documentation/CH55xE_DevStick_pic2.jpg)
![CH55xE_DevStick_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH55xE_DevStick/documentation/CH55xE_DevStick_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License.
(http://creativecommons.org/licenses/by-sa/3.0/)
