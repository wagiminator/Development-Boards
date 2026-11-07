Installing bootloader using CMSIS-DAP device
============================================
run the following command:
openocd -f cmsis-dap.cfg


Installing bootloader using SEGGER J-Link
============================================
run the following command (openocd):
openocd -f jlink.cfg

or (jlink commander):
JLinkExe -commanderscript flash.jlink


More information and source code
============================================
You have to modify the scripts if you are using
a differnet MCU than the SAM21E18A !!

- https://github.com/mattairtech/ArduinoCore-samd
- https://github.com/wagiminator
- http://openocd.org/
- http://www.segger.com/download-jlink.html
