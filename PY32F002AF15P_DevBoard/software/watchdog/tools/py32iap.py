#!/usr/bin/env python3
# ===================================================================================
# Project:   py32iap - IAP Programming Tool for PUYA PY32F0xx Microcontrollers
# Version:   v1.3
# Year:      2023
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple Python tool for flashing PY32F0xx (and maybe other PY32) microcontrollers
# via USB-to-serial converter utilizing the factory built-in embedded boot loader.
#
# Dependencies:
# -------------
# - pyserial
#
# Operating Instructions:
# -----------------------
# You need to install PySerial to use py32iap.
# Install it via "python3 -m pip install pyserial".
# You may need to install a driver for your USB-to-serial converter.
#
# Connect your USB-to-serial converter to your MCU:
# USB2SERIAL      PY32F0xx
#        RXD <--- PA2 or PA9  or PA14
#        TXD ---> PA3 or PA10 or PA15
#        VCC ---> VCC
#        GND ---> GND
#
# Set your MCU to boot mode by using ONE of the following methods:
# - Disconnect your USB-to-serial converter, pull BOOT0 pin (PF4) to VCC (or press
#   and hold the BOOT button, if your board has one), then connect the converter to
#   your USB port. BOOT0 pin (or BOOT button) can be released now.
# - Connect your USB-to-serial converter to your USB port. Pull BOOT0 pin (PF4)
#   to VCC, then pull nRST (PF2) shortly to GND (or press and hold the BOOT button,
#   then press and release the RESET button and then release the BOOT button, if
#   your board has them).
#
# Run "python3 py32iap.py -f firmware.bin".

# If the PID/VID of the USB-to-Serial converter is known, it can be defined here,
# which can make the auto-detection a lot faster. If not, comment out or delete.
PY_VID  = '1A86'
PY_PID  = '7523'

# Define BAUD rate here, range: 4800 - 1000000, default: 115200
PY_BAUD = 115200

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
    parser = argparse.ArgumentParser(description='Minimal command line interface for PY32 IAP')
    parser.add_argument('-u', '--unlock',   action='store_true', help='unlock chip (remove read protection)')
    parser.add_argument('-l', '--lock',     action='store_true', help='lock chip (set read protection)')
    parser.add_argument('-e', '--erase',    action='store_true', help='perform chip erase (implied with -f)')
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
        print('SUCCESS: Connection established via', isp.port + '.')
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    # Performing actions
    try:
        # Get chip info
        print('Getting chip info ...')
        isp.readinfo()
        if isp.pid == PY_CHIP_PID:
            print('SUCCESS: Found PY32F0xx with bootloader v' + isp.verstr + '.')
        else:
            print('WARNING: Chip with PID 0x%04x is not a PY32F0xx!' % isp.pid)

        # Unlock chip
        if args.unlock:
            print('Unlocking chip ...')
            isp.unlock()
            print('SUCCESS: Chip is unlocked.')
            print('INFO: Other options are ignored!')
            isp.close()
            print('DONE.')
            sys.exit(0)

        # Read option bytes and check, if chip is locked
        print('Reading OPTION bytes ...')
        isp.readoption()
        print('SUCCESS:', isp.optionstr + '.')

        # Perform chip erase
        if (args.erase) or (args.flash is not None):
            print('Performing chip erase ...')
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
        if isp.pid == PY_CHIP_PID and any( (args.rstoption, args.nrstgpio, args.nrstreset, args.lock) ):
            if args.rstoption:
                print('Setting OPTION bytes to default values ...')
                isp.resetoption()
            if args.nrstgpio:
                print('Setting nRST pin as GPIO in OPTION bytes ...')
                isp.nrst2gpio()
            if args.nrstreset:
                print('Setting nRST pin as RESET in OPTION bytes ...')
                isp.nrst2reset()
            if args.lock:
                print('Setting read protection in OPTION BYTES ...')
                isp.lock()
            print('Writing OPTION bytes ...')
            isp.writeoption()
            print('SUCCESS: OPTION bytes written.')
            isp.close()
        else:
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
        # BAUD rate:  4800 - 1000000bps (default: 115200), will be auto-detected
        # Data frame: 1 start bit, 8 data bit, 1 parity bit set to even, 1 stop bit
        super().__init__(baudrate = PY_BAUD, parity = serial.PARITY_EVEN, timeout = 1)
        self.identify()

    # Identify port of programmer and enter programming mode
    def identify(self):
        for p in comports():
            if (('PY_VID' not in globals()) or (PY_VID in p.hwid)) and (('PY_PID' not in globals()) or (PY_PID in p.hwid)):
                self.port = p.device
                try:
                    self.open()
                except:
                    continue
                self.reset_input_buffer()
                self.write([PY_SYNCH])
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
        return (len(reply) == 1 and reply[0] == PY_REPLY_ACK)

    #--------------------------------------------------------------------------------

    # Unlock (clear) chip and reset
    def unlock(self):
        self.sendcommand(PY_CMD_R_UNLOCK)
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
        self.ver    = self.readinfostream(PY_CMD_GET)[0]
        self.verstr = '%x.%x' % (self.ver >> 4, self.ver & 7)
        self.pid    = int.from_bytes(self.readinfostream(PY_CMD_PID), byteorder='big')

    # Read UID
    def readuid(self):
        return self.readflash(PY_UID_ADDR, 128)

    # Read OPTION bytes
    def readoption(self):
        try:
            self.option = list(self.readflash(PY_OPTION_ADDR, 16))
        except:
            raise Exception('Chip is locked')
        self.optionstr = 'OPTR: 0x%04x, SDKR: 0x%04x, WRPR: 0x%04x' % \
                         (( (self.option[ 0] << 8) + self.option[ 1], \
                            (self.option[ 4] << 8) + self.option[ 5], \
                            (self.option[12] << 8) + self.option[13] ))

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

    # Erase whole chip
    def erase(self):
        self.sendcommand(PY_CMD_ERASE)
        self.write(b'\xff\xff\x00')
        if not self.checkreply():
            raise Exception('Failed to erase chip')

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

# Device and Memory constants
PY_CHIP_PID     = 0x440
PY_BLOCKSIZE    = 128
PY_FLASH_ADDR   = 0x08000000
PY_CODE_ADDR    = 0x08000000
PY_SRAM_ADDR    = 0x20000000
PY_BOOT_ADDR    = 0x1fff0000
PY_UID_ADDR     = 0x1fff0e00
PY_OPTION_ADDR  = 0x1fff0e80
PY_CONFIG_ADDR  = 0x1fff0f00

# Command codes
PY_CMD_GET      = 0x00
PY_CMD_VER      = 0x01
PY_CMD_PID      = 0x02
PY_CMD_READ     = 0x11
PY_CMD_WRITE    = 0x31
PY_CMD_ERASE    = 0x44
PY_CMD_GO       = 0x21
PY_CMD_W_LOCK   = 0x63
PY_CMD_W_UNLOCK = 0x73
PY_CMD_R_LOCK   = 0x82
PY_CMD_R_UNLOCK = 0x92

# Reply codes
PY_REPLY_ACK    = 0x79
PY_REPLY_NACK   = 0x1f
PY_REPLY_BUSY   = 0xaa

# Other codes
PY_SYNCH        = 0x7f

# Default option bytes
PY_OPTION_DEFAULT = b'\xaa\xbe\x55\x41\xff\x00\x00\xff\xff\xff\xff\xff\xff\xff\x00\x00'

# ===================================================================================

if __name__ == "__main__":
    _main()
