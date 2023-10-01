#!/usr/bin/env python3
# ===================================================================================
# Project:   puyaisp - USB Programming Tool for PUYA PY32F0xx Microcontrollers
# Version:   v1.1
# Year:      2023
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple Python tool for flashing PUYA microcontrollers (PY32F0xx) via USB-to-serial
# converter utilizing the factory built-in embedded boot loader.
#
# Dependencies:
# -------------
# - pyserial
#
# Operating Instructions:
# -----------------------
# You need to install PySerial to use puyaisp.
# Install it via "python3 -m pip install pyserial".
# You may need to install a driver for your USB-to-serial converter.
#
# Connect your USB-to-serial converter to your MCU:
# USB2SERIAL      PY32F0xx
#        RXD <--- PA2 or PA9  or PA14
#        TXD ---> PA3 or PA10 or PA15
#        VCC ---> VCC
#        GND ---> GND
# Set your MCU to boot mode by using ONE of the following methods:
# 1. Disconnect your USB-to-serial converter, pull BOOT0 pin (PF4) to VCC (or press
#    and hold the BOOT button, if your board has one), then connect the converter to
#    your USB port. BOOT0 pin (or BOOT button) can be released now.
# 2. Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin (PF4)
#    to VCC, then pull nRST (PF2) shortly to GND (or press and hold the BOOT button,
#    then press and release the RESET button and then release the BOOT button, if
#    your board has them).
# Run "python3 puyaisp.py -f firmware.bin".

import sys
import argparse
import serial
from serial import Serial
from serial.tools.list_ports import comports

# If the PID/VID of the USB-to-Serial converter is known, it can be defined here,
# which can make the auto-detection a lot faster.
VID = '1A86'
PID = '7523'

# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Minimal command line interface for PY32F0xx programming')
    parser.add_argument('-u', '--unlock',   action='store_true', help='unlock chip (remove read protection)')
    parser.add_argument('-l', '--lock',     action='store_true', help='lock chip (set read protection)')
    parser.add_argument('-e', '--erase',    action='store_true', help='perform a whole chip erase')
    parser.add_argument('-o', '--rstoption',action='store_true', help='reset option bytes')
    parser.add_argument('-G', '--nrstgpio', action='store_true', help='make nRST pin a GPIO pin')
    parser.add_argument('-R', '--nrstreset',action='store_true', help='make nRST pin a RESET pin')
    parser.add_argument('-f', '--flash',    help='write BIN file to flash and verify')
    args = parser.parse_args(sys.argv[1:])

    # Check arguments
    if not any( (args.rstoption, args.unlock, args.lock, args.erase, args.nrstgpio, args.nrstreset, args.flash) ):
        print('No arguments - no action!')
        sys.exit(0)

    # Establish connection to MCU via USB-to-serial converter
    try:
        print('Connecting to MCU via USB-to-serial converter ...')
        isp = Programmer()
        print('SUCCESS: Connection established.')
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    # Performing actions
    try:
        # Get chip info
        print('Reading chip info ...')
        isp.readinfo()
        if isp.locked and not args.unlock:
            raise Exception('Chip is locked')

        # Unlock chip
        if args.unlock:
            print('Unlocking chip ...')
            isp.unlock()
            print('SUCCESS: Chip is unlocked.')
            print('INFO: Other options are ignored!')
            isp.close()
            print('DONE.')
            sys.exit(0)

        # Perform chip erase
        if (args.erase) or (args.flash is not None):
            print('Performing whole chip erase ...')
            isp.erase()
            print('SUCCESS: Chip is erased.')

        # Flash binary file
        if args.flash is not None:
            print('Flashing', args.flash, 'to MCU ...')
            with open(args.flash, 'rb') as f: data = f.read()
            isp.writeflash(PY_CODE_ADDR, data)
            print('Verifying ...')
            isp.verifyflash(PY_CODE_ADDR, data)
            print('SUCCESS:', len(data), 'bytes written and verified.')

        # Manipulate OPTION bytes
        if any( (args.rstoption, args.nrstgpio, args.nrstreset, args.lock) ):
            if args.rstoption:
                print('Setting OPTION bytes to default values ...')
                isp.resetoption()
            if args.nrstgpio:
                print('Setting nRST pin as GPIO ...')
                isp.nrst2gpio()
            if args.nrstreset:
                print('Setting nRST pin as RESET ...')
                isp.nrst2reset()
            if args.lock:
                print('Setting read protection ...')
                isp.lock()
            print('Writing OPTION bytes ...')
            isp.writeoption()
            print('SUCCESS: OPTION bytes written.')

        # Exit programming and start firmware
        if not any( (args.rstoption, args.nrstgpio, args.nrstreset, args.lock) ):
            isp.run()
        isp.close()

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
        super().__init__(baudrate = 115200, timeout = 1, stopbits = serial.STOPBITS_TWO)
        self.identify()

    # Identify port of programmer and enter programming mode
    def identify(self):
        for p in comports():
            if (('VID' not in globals()) or (VID in p.hwid)) and (('PID' not in globals()) or (PID in p.hwid)):
                self.port = p.device
                try:
                    self.open()
                except:
                    continue
                self.reset_input_buffer()
                self.write([0x7f])
                if not self.checkreply():
                    self.close()
                    continue
                self.write([0x7f])
                self.write([0x7f])
                reply = self.read(1)
                if len(reply) == 0 or reply[0] != 0x1f:
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
        return (len(reply) == 1 and reply[0] == PY_REPLY_OK)

    #--------------------------------------------------------------------------------

    # Unlock (clear) chip and reset
    def unlock(self):
        self.sendcommand(PY_CMD_UNLOCK)
        if not self.checkreply():
            raise Exception('Failed to unlock chip')

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
        # still need to find out meanings
        self.info = self.readinfostream(0x00)
        self.pid  = int.from_bytes(self.readinfostream(0x02), byteorder='big')
        self.locked = False
        try:
            self.readoption()
        except:
            self.locked = True

    # Read UID
    def readuid(self):
        return self.readflash(PY_UID_ADDR, 128)

    # Read OPTION bytes
    def readoption(self):
        self.option = list(self.readflash(PY_OPTION_ADDR, 16))

    # Write OPTION bytes
    def writeoption(self):
        self.writeflash(PY_OPTION_ADDR, self.option)

    # Reset OPTION bytes
    def resetoption(self):
        self.option = list(PY_OPTION_DEFAULT)

    # Set read protection in OPTION bytes
    def lock(self):
        self.option[0]  = 0x55
        self.option[2]  = 0xaa

    # Set nRST pin as GPIO in OPTION bytes
    def nrst2gpio(self):
        self.option[1] |= 0x40
        self.option[3] &= 0xbf

    # Set nRST pin as RESET in OPTION bytes
    def nrst2reset(self):
        self.option[1] &= 0xbf
        self.option[3] |= 0x40

    #--------------------------------------------------------------------------------

    # Erase all
    def erase(self):
        self.sendcommand(PY_CMD_ERASE)
        self.write(b'\xff\xff\x00')
        if not self.checkreply():
            raise Exception('Failed to erase chip')

    # Erase necessary pages
    def erasepages(self):
        self.sendcommand(PY_CMD_ERASE)
        self.write(b'\x10\x02\x00\x00\x00\x01\x00\x02\x11')
        if not self.checkreply():
            raise Exception('Failed to erase pages')

    # Erase necessary sectors
    def erasesectors(self):
        self.sendcommand(PY_CMD_ERASE)
        self.write(b'\x20\x00\x00\x00\x20')
        if not self.checkreply():
            raise Exception('Failed to erase sectors')

    #--------------------------------------------------------------------------------

    # Read flash
    def readflash(self, addr, size):
        data = bytes()
        while size > 0:
            blocksize = size
            if blocksize > PY_BLOCKSIZE: blocksize = PY_BLOCKSIZE
            self.sendcommand(PY_CMD_READ)
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
            blocksize = size
            if blocksize > PY_BLOCKSIZE: blocksize = PY_BLOCKSIZE
            block = data[:blocksize]
            parity = blocksize - 1
            for x in range(blocksize):
                parity ^= block[x]
            self.sendcommand(PY_CMD_WRITE)
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

    #--------------------------------------------------------------------------------

    # Jump to address
    def gotoaddress(self, addr):
        self.sendcommand(PY_CMD_GO)
        self.sendaddress(addr)

    # Start firmware
    def run(self):
        self.gotoaddress(PY_CODE_ADDR)

# ===================================================================================
# Device Constants
# ===================================================================================

# Memory constants
PY_BLOCKSIZE    = 0x80
PY_FLASH_ADDR   = 0x08000000
PY_CODE_ADDR    = 0x08000000
PY_SRAM_ADDR    = 0x20000000
PY_BOOT_ADDR    = 0x1fff0000
PY_UID_ADDR     = 0x1fff0e00
PY_OPTION_ADDR  = 0x1fff0e80
PY_CONFIG_ADDR  = 0x1fff0f00

# Command codes
PY_CMD_READ     = 0x11
PY_CMD_GO       = 0x21
PY_CMD_WRITE    = 0x31
PY_CMD_ERASE    = 0x44
PY_CMD_UNLOCK   = 0x92

# Reply codes
PY_REPLY_OK     = 0x79

# Other stuff
PY_OPTION_DEFAULT = b'\xaa\xbe\x55\x41\xff\x00\x00\xff\xff\xff\xff\xff\xff\xff\x00\x00'

# ===================================================================================

if __name__ == "__main__":
    _main()
