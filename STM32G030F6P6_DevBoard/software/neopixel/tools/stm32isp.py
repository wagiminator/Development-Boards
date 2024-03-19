#!/usr/bin/env python3
# ===================================================================================
# Project:   stm32isp - Programming Tool for some STM32 Microcontrollers
# Version:   v0.8
# Year:      2023
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Python tool for flashing some entry-level STM32 microcontrollers via USB-to-serial 
# converter utilizing the factory built-in UART boot loader. Supported devices:
# - STM32C011/031
# - STM32F03xx4/6
# - STM32G03x/04x
# - STM32L01x/02x
# !!! THIS TOOL IS IN AN EARLY STAGE OF DEVELOPMENT !!!
#
# Dependencies:
# -------------
# - pyserial
#
# Operating Instructions:
# -----------------------
# You need to install PySerial to use stm32isp.
# Install it via "python3 -m pip install pyserial".
# You may need to install a driver for your USB-to-serial converter.
#
# On some STM32, the BOOT0 pin is disabled by default. As soon as the chip is brand 
# new and/or the main flash memory is deleted, this is not a problem, the embedded 
# bootloader will automatically start. Using this stm32isp tool will automatically 
# activate the BOOT0 pin so that it can also be used in the following. However, if 
# the chip has already been written to before using a different software tool, it 
# is likely that the bootloader can no longer be activated via the BOOT0 pin. In 
# this case, the bit nBOOT_SEL in the User Option Bytes must be deleted (set to 0) 
# using an SWD programmer (e.g. ST-Link) and appropriate software.
#
# Connect your USB-to-serial converter to your MCU:
# USB2SERIAL      STM32C011/031
#        RXD <--- PA9  (PA11)
#        TXD ---> PA10 (PA12)
#        VCC ---> VCC
#        GND ---> GND
#
# USB2SERIAL      STM32F03xx4/6
#        RXD <--- PA9  or PA14
#        TXD ---> PA10 or PA15
#        VCC ---> VCC
#        GND ---> GND
#
# USB2SERIAL      STM32G03x/04x
#        RXD <--- PA2 or PA9
#        TXD ---> PA3 or PA10
#        VCC ---> VCC
#        GND ---> GND
#
# USB2SERIAL      STM32L01x/02x
#        RXD <--- PA2 or PA9
#        TXD ---> PA3 or PA10
#        VCC ---> VCC
#        GND ---> GND
#
# Set your MCU to boot mode by using the following method:
# - Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin (PA14)
#   to VCC, then pull nRST shortly to GND (or press and hold the BOOT button,
#   then press and release the RESET button and then release the BOOT button, if
#   your board has them).
#
# Run "python3 stm32isp.py -f firmware.bin".

# If the PID/VID of the USB-to-Serial converter is known, it can be defined here,
# which can make the auto-detection a lot faster. If not, comment out or delete.
#ST_VID  = '1A86'
#ST_PID  = '7523'

# Define BAUD rate here, range: 1200 - 115200, default: 115200
ST_BAUD = 115200

# Libraries
import sys
import argparse
import serial
from serial import Serial
from serial.tools.list_ports import comports

# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Minimal command line interface for STM32 ISP')
    parser.add_argument('-u', '--unlock',   action='store_true', help='unlock chip (remove read protection)')
    parser.add_argument('-l', '--lock',     action='store_true', help='lock chip (set read protection)')
    parser.add_argument('-e', '--erase',    action='store_true', help='perform a chip erase (implied with -f)')
    parser.add_argument('-f', '--flash',    help='write BIN file to flash and verify')
    args = parser.parse_args(sys.argv[1:])

    # Check arguments
    if not any( (args.unlock, args.lock, args.erase, args.flash) ):
        print('No arguments - no action!')
        sys.exit(0)

    # Establish connection to MCU via USB-to-serial converter
    try:
        print('Connecting to MCU via USB-to-serial converter ...')
        isp = Programmer()
        print('SUCCESS: Connection established via', isp.port + '.')
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    # Performing actions
    try:
        # Get chip info
        print('Getting chip info ...')
        isp.readinfo()
        print('SUCCESS: Found', isp.chipname, 'with bootloader v' + isp.verstr + '.')

        # Check if chip is locked, unlock if so; erase chip
        if not isp.readoption():
            print('Chip is locked, unlocking and erasing ...')
            isp.unlock()
            isp.sendcommand(ST_SYNCH)
            isp.readoption()
            print('SUCCESS: Chip is unlocked and erased.')
        else:
            if args.unlock:
                print('INFO: Chip is already unlocked.')
            if (args.erase) or (args.flash is not None):
                print('Performing chip erase ...')
                isp.erase()
                print('SUCCESS: Chip is erased.')

        # Flash binary file
        if args.flash is not None:
            print('Flashing', args.flash, 'to', isp.chipname, '...')
            with open(args.flash, 'rb') as f: data = f.read()
            isp.writeflash(ST_CODE_ADDR, data)
            print('Verifying ...')
            isp.verifyflash(ST_CODE_ADDR, data)
            print('SUCCESS:', len(data), 'bytes written and verified.')

        # Enable BOOT0 pin in OPTION bytes (nBOOT_SEL = 0) for STM32C/G
        if (isp.pid in (0x443, 0x453, 0x466)) and isp.checkbootpin():
            print('Enabling BOOT0 pin in OPTION bytes ...')
            isp.enablebootpin()
            print('SUCCESS: Modified OPTION bytes written.')
            isp.close()

        # Lock chip (set readout protection)
        if args.lock:
            print('Locking chip ...')
            isp.lock()
            print('SUCCESS: Chip is locked.')
            print('INFO: A power-on reset may need to be performed.')
            isp.close()

        # Run firmware
        if isp.isOpen():
            isp.run()

    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        isp.close()
        sys.exit(1)

    print('DONE.')
    sys.exit(0)

# ===================================================================================
# Programmer Class
# ===================================================================================

class Programmer(Serial):
    def __init__(self):
        # BAUD rate:  1200 - 115200bps (default: 115200), will be auto-detected
        # Data frame: 1 start bit, 8 data bit, 1 parity bit set to even, 1 stop bit
        super().__init__(baudrate = ST_BAUD, parity = serial.PARITY_EVEN, timeout = 1)
        self.identify()

    # Identify port of programmer and enter programming mode
    def identify(self):
        for p in comports():
            if (('ST_VID' not in globals()) or (ST_VID in p.hwid)) and (('ST_PID' not in globals()) or (ST_PID in p.hwid)):
                self.port = p.device
                try:
                    self.open()
                except:
                    continue
                self.reset_input_buffer()
                self.write([ST_SYNCH])
                if not self.checkreply():
                    self.close()
                    continue
                return
        raise Exception('No MCU in boot mode found')

    # Send command
    def sendcommand(self, command):
        self.write([command, command ^ 0xff])
        if not self.checkreply():
            raise Exception('Device has not acknowledged the command 0x%02x' % command)

    # Send address
    def sendaddress(self, addr):
        stream = addr.to_bytes(4, byteorder='big')
        parity = 0x00
        for x in range(4):
            parity ^= stream[x]
        self.write(stream)
        self.write([parity])
        if not self.checkreply():
            raise Exception('Failed to send address')

    # Check if device acknowledged
    def checkreply(self):
        reply = self.read(1)
        return (len(reply) == 1 and reply[0] == ST_REPLY_ACK)

    #--------------------------------------------------------------------------------

    # Read info stream
    def readinfostream(self, command):
        self.sendcommand(command)
        size = self.read(1)[0]
        stream = self.read(size + 1)
        if not self.checkreply():
            raise Exception('Failed to read info')
        return stream

    # Get chip info
    def readinfo(self):
        self.info   = self.readinfostream(ST_CMD_GET)
        self.ver    = self.info[0]
        self.verstr = '%x.%x' % (self.ver >> 4, self.ver & 7)
        self.cmds   = list(self.info[1:])
        self.pid    = int.from_bytes(self.readinfostream(ST_CMD_PID), byteorder='big')
        self.device = None

        # Find device in dictionary
        for d in DEVICES:
            if d['id'] == self.pid:
                self.device = d
        if self.device is None:
            raise Exception('Unsupported chip (ID: 0x%04x)' % self.chipid)

        # Set parameters according to dictionary
        self.chipname       = self.device['name']
        self.option_address = self.device['opt_addr']

    # Read OPTION bytes
    def readoption(self):
        try:
            self.option = list(self.readflash(self.option_address, 8))
        except:
            return False
        if self.pid == 0x457:
            self.optionbytes =  int.from_bytes(self.option[0:2], byteorder='little') \
                             | (int.from_bytes(self.option[4:6], byteorder='little') << 16)
        else:
            self.optionbytes = int.from_bytes(self.option[:4], byteorder='little')
        return True

    # Check if BOOT0 pin ist disabled in OPTION bytes (STM32C/G only)
    def checkbootpin(self):
        return ((self.option[3] & 0x01) == 0x01)

    # Enable BOOT0 pin in OPTION bytes (nBOOT_SEL = 0), STM32C/G only
    def enablebootpin(self):
        if self.pid in (0x443, 0x453, 0x466):
            self.option[3] = (self.option[3] & 0b11111000) | 0b00000110
            for x in range(4):
                self.option[x+4] = self.option[x] ^ 0xff
            self.writeflash(self.option_address, self.option)

    # Unlock chip (remove readout protection), erase and reset
    def unlock(self):
        self.sendcommand(ST_CMD_R_UNLOCK)
        if not self.checkreply():
            raise Exception('Failed to unlock chip')
            
    # Lock chip (set readout protection)
    def lock(self):
        self.sendcommand(ST_CMD_R_LOCK)
        if not self.checkreply():
            raise Exception('Failed to lock chip')

    # Start firmware and disconnect
    def run(self):
        self.sendcommand(ST_CMD_GO)
        self.sendaddress(ST_CODE_ADDR)
        self.close()

    #--------------------------------------------------------------------------------

    # Erase whole chip
    def erase(self):
        if self.pid == 0x457:
            self.erasepages(128)
            return
        elif ST_CMD_ERASE_X in self.cmds:
            self.sendcommand(ST_CMD_ERASE_X)
            self.write(b'\xff\xff\x00')
        else:
            self.sendcommand(ST_CMD_ERASE)
            self.write(b'\xff\x00')
        if not self.checkreply():
            raise Exception('Failed to erase chip')

    # Erase number of pages
    def erasepages(self, pagecount):
        pagecount -= 1
        self.sendcommand(ST_CMD_ERASE_X)
        self.write([pagecount >> 16, pagecount & 0xff])
        parity = (pagecount >> 16) ^ (pagecount & 0xff)
        for x in range(pagecount + 1):
            self.write([x >> 16, x & 0xff])
            parity ^= (x >> 16) ^ (x & 0xff)
        self.write([parity])
        if not self.checkreply():
            raise Exception('Failed to erase pages')

    # Read flash
    def readflash(self, addr, size):
        data = bytes()
        while size > 0:
            blocksize = min(size, ST_PAGE_SIZE)
            self.sendcommand(ST_CMD_READ)
            self.sendaddress(addr)
            self.sendcommand(blocksize - 1)
            data += self.read(blocksize)
            addr += blocksize
            size -= blocksize
        return data

    # Write to flash
    def writeflash(self, addr, data):
        size = len(data)
        while size > 0:
            blocksize = min(size, ST_PAGE_SIZE)
            block     = data[:blocksize]
            parity    = blocksize - 1
            for x in range(blocksize):
                parity ^= block[x]
            self.sendcommand(ST_CMD_WRITE)
            self.sendaddress(addr)
            self.write([blocksize - 1])
            self.write(block)
            self.write([parity])
            if not self.checkreply():
                raise Exception('Failed to write to address 0x%08x' % addr)
            data  = data[blocksize:]
            addr += blocksize
            size -= blocksize

    # Verify flash
    def verifyflash(self, addr, data):
        flash = self.readflash(addr, len(data))
        if set(flash) != set(data):
            raise Exception('Verification failed')

    # Pad data
    def paddata(self, data, pagesize):
        if (len(data) % pagesize) > 0:
            data += b'\xff' * (pagesize - (len(data) % pagesize))
        return data

# ===================================================================================
# Device Constants
# ===================================================================================

# Device and Memory constants
ST_PAGE_SIZE    = 256
ST_FLASH_ADDR   = 0x08000000
ST_CODE_ADDR    = 0x08000000
ST_SRAM_ADDR    = 0x20000000

# Command codes
ST_CMD_GET      = 0x00
ST_CMD_VER      = 0x01
ST_CMD_PID      = 0x02
ST_CMD_READ     = 0x11
ST_CMD_WRITE    = 0x31
ST_CMD_ERASE    = 0x43
ST_CMD_ERASE_X  = 0x44
ST_CMD_GO       = 0x21
ST_CMD_W_LOCK   = 0x63
ST_CMD_W_UNLOCK = 0x73
ST_CMD_R_LOCK   = 0x82
ST_CMD_R_UNLOCK = 0x92

# Reply codes
ST_REPLY_ACK    = 0x79
ST_REPLY_NACK   = 0x1f
ST_REPLY_BUSY   = 0xaa

# Other codes
ST_SYNCH        = 0x7f

# ===================================================================================
# Device definitions
# ===================================================================================

DEVICES = [
    {'name': 'STM32C011xx',   'id': 0x443, 'opt_addr': 0x1fff7800, 'opt_default': 0xfffffeaa},
    {'name': 'STM32C031xx',   'id': 0x453, 'opt_addr': 0x1fff7800, 'opt_default': 0xfffffeaa},
    {'name': 'STM32G03x/04x', 'id': 0x466, 'opt_addr': 0x1fff7800, 'opt_default': 0xfffffeaa},
    {'name': 'STM32L01x/02x', 'id': 0x457, 'opt_addr': 0x1ff80000, 'opt_default': 0x807000aa},
    {'name': 'STM32F03xx4/6', 'id': 0x444, 'opt_addr': 0x1ffff800, 'opt_default': 0x00ff55aa}
]

# ===================================================================================

if __name__ == "__main__":
    _main()
