# ===================================================================================
# Project:  Touchkey Raw Value Demo for CH551, CH552, CH554
# Author:   Stefan Wagner
# Year:     2023
# URL:      https://github.com/wagiminator
# ===================================================================================         
# Type "make help" in the command line.
# ===================================================================================

# Files and Folders
MAINFILE   = touchraw.c
TARGET     = touchraw
INCLUDE    = src
TOOLS      = tools

# Microcontroller Settings
FREQ_SYS   = 16000000
XRAM_LOC   = 0x0100
XRAM_SIZE  = 0x0300
CODE_SIZE  = 0x3800

# Toolchain
CC         = sdcc
OBJCOPY    = objcopy
PACK_HEX   = packihx
ISPTOOL   ?= python3 $(TOOLS)/chprog.py $(TARGET).bin

# Compiler Flags
CFLAGS  = -mmcs51 --model-small --no-xinit-opt -DF_CPU=$(FREQ_SYS) -I$(INCLUDE) -I.
CFLAGS += --xram-size $(XRAM_SIZE) --xram-loc $(XRAM_LOC) --code-size $(CODE_SIZE)
CFILES  = $(MAINFILE) $(wildcard $(INCLUDE)/*.c)
RFILES  = $(CFILES:.c=.rel)
CLEAN   = rm -f *.ihx *.lk *.map *.mem *.lst *.rel *.rst *.sym *.asm *.adb

# Symbolic Targets
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

$(TARGET).ihx: $(RFILES)
	@echo "Building $(TARGET).ihx ..."
	@$(CC) $(notdir $(RFILES)) $(CFLAGS) -o $(TARGET).ihx

$(TARGET).hex: $(TARGET).ihx
	@echo "Building $(TARGET).hex ..."
	@$(PACK_HEX) $(TARGET).ihx > $(TARGET).hex

$(TARGET).bin: $(TARGET).ihx
	@echo "Building $(TARGET).bin ..."
	@$(OBJCOPY) -I ihex -O binary $(TARGET).ihx $(TARGET).bin
	
flash: $(TARGET).bin size removetemp
	@echo "Uploading to CH55x ..."
	@$(ISPTOOL)

all: $(TARGET).bin $(TARGET).hex size

hex: $(TARGET).hex size removetemp

bin: $(TARGET).bin size removetemp

bin-hex: $(TARGET).bin $(TARGET).hex size removetemp

install: flash

size:
	@echo "------------------"
	@echo "FLASH: $(shell awk '$$1 == "ROM/EPROM/FLASH"      {print $$4}' $(TARGET).mem) bytes"
	@echo "IRAM:  $(shell awk '$$1 == "Stack"           {print 248-$$10}' $(TARGET).mem) bytes"
	@echo "XRAM:  $(shell awk '$$1 == "EXTERNAL" {print $(XRAM_LOC)+$$5}' $(TARGET).mem) bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(TARGET).hex $(TARGET).bin
