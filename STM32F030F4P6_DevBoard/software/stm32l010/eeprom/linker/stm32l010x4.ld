ENTRY( Reset_Handler )

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 16K
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}

SECTIONS
{
  .vectors  : { KEEP(*(.isr_vector)) }  > FLASH
  .text     : { *(.text*) }             > FLASH
  .rodata   : { *(.rodata*) }           > FLASH

  .data : {
    . = ALIGN(4);
    _sdata = .;
    *(.first_data)
    *(.data SORT(.data.*))
    . = ALIGN(4);
    _edata = .;
  } > RAM AT > FLASH

  .bss :
  {
    _sbss = .;
    *(.bss SORT(.bss.*))
    *(COMMON*)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  _end = .;
  _sidata = LOADADDR(.data);
  _estack = ORIGIN(RAM) + LENGTH(RAM);
}
