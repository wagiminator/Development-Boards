# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  CH55x Example
# Author:   Stefan Wagner
# Year:     2026
# URL:      https://github.com/wagiminator
# ===================================================================================         
# Install toolchain:
#   sudo apt install build-essential sdcc
#   sudo apt install python3 python3-pip
#   python3 -m pip install pyusb chprog
#
# Provide access permission to USB bootloader:
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
#   sudo udevadm control --reload-rules
#
# Type "make flash" in the command line.
# ===================================================================================

# Files and Folders
TARGET    = usb2serial
INCLUDE   = include
SOURCE    = src
BIN       = bin
TOOLS     = tools

# Microcontroller Settings
FREQ_SYS  = 24000000
XRAM_LOC  = 0x0100
XRAM_SIZE = 0x0300
CODE_SIZE = 0x3800
# CH551: XRAM_SIZE = 0x0100, CODE_SIZE = 0x2800

# Toolchain
CC        = sdcc
OBJCOPY   = objcopy
PACK_HEX  = packihx
ISPTOOL  ?= chprog $(BIN)/$(TARGET).bin

# Compiler Flags
CFLAGS  = -mmcs51 --model-small --no-xinit-opt --opt-code-size -DF_CPU=$(FREQ_SYS) -I$(INCLUDE) -I$(SOURCE) -I.
CFLAGS += --xram-size $(XRAM_SIZE) --xram-loc $(XRAM_LOC) --code-size $(CODE_SIZE)
CFILES  = $(wildcard ./*.c) $(wildcard $(SOURCE)/*.c)
RFILES  = $(CFILES:.c=.rel)
CLEAN   = rm -f *.ihx *.lk *.map *.mem *.lst *.rel *.rst *.sym *.asm *.adb $(BIN)/*.ihx $(BIN)/*.lk $(BIN)/*.map $(BIN)/*.mem

# Symbolic Targets
.PHONY: help flash all hex bin bin-hex install size removetemp clean

help:
	@echo "Use the following commands:"
	@echo "make all     compile, build and keep all files"
	@echo "make hex     compile and build $(TARGET).hex"
	@echo "make bin     compile and build $(TARGET).bin"
	@echo "make flash   compile, build and upload $(TARGET).bin to device"
	@echo "make clean   remove all build files"

%.rel : %.c
	@echo "Compiling $< ..."
	@$(CC) -c $(CFLAGS) $<

$(BIN)/$(TARGET).ihx: $(RFILES)
	@echo "Building $(BIN)/$(TARGET).ihx ..."
	@mkdir -p $(BIN)
	@$(CC) $(notdir $(RFILES)) $(CFLAGS) -o $(BIN)/$(TARGET).ihx

$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).ihx
	@echo "Building $(BIN)/$(TARGET).hex ..."
	@$(PACK_HEX) $(BIN)/$(TARGET).ihx > $(BIN)/$(TARGET).hex

$(BIN)/$(TARGET).bin: $(BIN)/$(TARGET).ihx
	@echo "Building $(BIN)/$(TARGET).bin ..."
	@$(OBJCOPY) -I ihex -O binary $(BIN)/$(TARGET).ihx $(BIN)/$(TARGET).bin
	
flash: $(BIN)/$(TARGET).bin size removetemp
	@echo "Uploading to CH55x ..."
	@$(ISPTOOL)

all: $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex size

hex: $(BIN)/$(TARGET).hex size removetemp

bin: $(BIN)/$(TARGET).bin size removetemp

bin-hex: $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex size removetemp

install: flash

size:
	@echo "------------------"
	@echo "FLASH: $(shell awk '$$1 == "ROM/EPROM/FLASH" {print $$(NF-1)}' $(BIN)/$(TARGET).mem) bytes"
	@echo "IRAM:  $(shell awk '$$1 == "Stack" {print 248-$$10}' $(BIN)/$(TARGET).mem) bytes"
	@echo "XRAM:  $(shell awk '$$1 == "EXTERNAL" {print $$(NF-1)}' $(BIN)/$(TARGET).mem) bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).bin
