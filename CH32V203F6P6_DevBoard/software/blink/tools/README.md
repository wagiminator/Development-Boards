# Programming Tools
There are two ways to program the microcontroller: via USB using the built-in bootloader and via the serial 2-wire debug interface using a WCH-Link programming device.

# Built-in USB Bootloader
## Installing Drivers for the Bootloader
On Linux you do not need to install a driver. However, by default Linux will not expose enough permission to upload your code with the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

For Windows, you need the [CH372 driver](http://www.wch-ic.com/downloads/CH372DRV_EXE.html). Alternatively, you can also use the [Zadig Tool](https://zadig.akeo.ie/) to install the correct driver. Here, click "Options" -> "List All Devices" and select the USB module. Then install the libusb-win32 driver. To do this, the board must be connected and the CH55x must be in bootloader mode.

## Entering Bootloader Mode
The bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT0 button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT0 button can be released and new firmware can be uploaded via USB. Alternatively, you can leave the board connected to the USB port, press and hold the BOOT0 button, press and release the RESET button and then release the BOOT0 button to enter the bootloader mode.

## WCHISPTool
WCH offers the free but closed-source software [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) to upload firmware with Windows via the USB bootloader.

## chprog.py
You can also use chprog.py, a simple Python tool provided in this folder, to flash the microcontroller via the built-in USB bootloader. The code for this tool is derived from Frank Zago's [isp55e0](https://github.com/frank-zago/isp55e0).

In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PyUSB](https://github.com/pyusb/pyusb) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyusb
```

```
Usage example:
python3 chprog.py firmware.bin
```

## Alternative Software Tools
- [isp55e0](https://github.com/frank-zago/isp55e0)
- [wchisp](https://github.com/ch32-rs/wchisp)

# Serial 2-Wire Debug Interface
## WCH-LinkE
To program the microcontroller via the serial debug interface, you will need a special programming device. The [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name) is a suitable option for this purpose and can be purchased commercially for around $3. This debugging tool is compatible with most WCH RISC-V and ARM-based microcontrollers.

To use the WCH-Link on Linux, you need to grant access permissions beforehand by executing the following commands:
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
```

On Windows, if you need to you can install the WinUSB driver over the WCH interface 1.

To upload firmware, you should make the following connections to the WCH-LinkE:

```
WCH-LinkE      MCU Board
+-------+      +------+
|  SWCLK| ---> |CLK   |
|  SWDIO| <--> |DIO   |
|    GND| ---> |GND   |
|    3V3| ---> |3V3   |
+-------+      +------+
```

If the blue LED on the WCH-LinkE remains illuminated once it is connected to the USB port, it means that the device is currently in ARM mode and must be switched to RISC-V mode initially. There are a few ways to accomplish this:
- You can utilize the Python tool called rvmode.py (refer to the instructions below).
- Alternatively, you can select "WCH-LinkRV" in the software provided by WCH, such as MounRiver Studio or WCH-LinkUtility.
- Another option is to hold down the ModeS button on the device while plugging it into the USB port.

More information can be found in the [WCH-Link User Manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html).

## WCH-LinkUtility
WCH offers the free but closed-source software [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html) to upload firmware with Windows using the WCH-LinkE.

## rvprog.py
You can also use rvprog.py, a simple Python tool provided in this folder, to flash the microcontroller via the WCH-LinkE or compatible programmers/debuggers. The code for this tool is derived from CNLohr's [minichlink](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink).

In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PyUSB](https://github.com/pyusb/pyusb) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyusb
```

```
Usage: rvprog.py [-h] [-a] [-v] [-b] [-u] [-l] [-e] [-G] [-R] [-f FLASH]

Optional arguments:
  -h, --help                show help message and exit
  -a, --armmode             switch WCH-Link to ARM mode
  -v, --rvmode              switch WCH-Link to RISC-V mode
  -b, --unbrick             unbrick chip
  -u, --unlock              unlock chip (remove read protection)
  -l, --lock                lock chip (set read protection)
  -e, --erase               perform a whole chip erase
  -G, --pingpio             make nRST pin a GPIO pin
  -R, --pinreset            make nRST pin a reset pin
  -f FLASH, --flash FLASH   write BIN file to flash

Example:
python3 rvprog.py -f firmware.bin
```

## rvmode.py
The Python tool rvmode.py, which can be found in this folder, provides a simple way to switch the WCH-LinkE to RISC-V mode (LinkRV mode). Python3 and PyUSB must be installed on your system for the tool to work.
```
Usage example:
python3 rvmode.py
```

## Alternative Software Tools
- [minichlink](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink)
- [wlink](https://github.com/ch32-rs/wlink)
