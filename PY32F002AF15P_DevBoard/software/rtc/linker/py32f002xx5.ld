ENTRY( Reset_Handler )

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 20K
  RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 3K
}

SECTIONS
{
  .vectors :
  {
    *(.vectors);
    . = ALIGN(4);
  } > FLASH

  .init :
  { 
    . = ALIGN(4);
    _sinit = .;
    KEEP(*(SORT_NONE(.init)))
    . = ALIGN(4);
    _einit = .;
  } > FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.rodata)
    *(.rodata*)
    *(.eh_frame)
    *(.eh_frame*)
    . = ALIGN(4);
  } > FLASH

  .fini :
  {
    KEEP(*(SORT_NONE(.fini)))
    . = ALIGN(4);
  } > FLASH

  PROVIDE(_etext = .);

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  _sexidx = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH
  _eexidx = .;

  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } > FLASH
  
  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(.init_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o) .ctors))
    PROVIDE_HIDDEN(__init_array_end = .);
  } > FLASH
  
  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP(*(.fini_array EXCLUDE_FILE(*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o) .dtors))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } > FLASH
  
  .ctors :
  {
    KEEP(*crtbegin.o(.ctors))
    KEEP(*crtbegin?.o(.ctors))
    KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o) .ctors))
    KEEP(*(SORT(.ctors.*)))
    KEEP(*(.ctors))
  } > FLASH
  
  .dtors :
  {
    KEEP(*crtbegin.o(.dtors))
    KEEP(*crtbegin?.o(.dtors))
    KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o) .dtors))
    KEEP(*(SORT(.dtors.*)))
    KEEP(*(.dtors))
  } > FLASH

  .dalign :
  {
    . = ALIGN(4);
    PROVIDE(_data_vma = .);
  } > RAM

  .dlalign :
  {
    . = ALIGN(4); 
    PROVIDE(_data_lma = .);
  } > FLASH

  .data :
  {
    . = ALIGN(4);
    PROVIDE(_sdata = .);
    *(.data)
    *(.data*)
    . = ALIGN(4);
    PROVIDE(_edata = .);
  } > RAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    PROVIDE(_sbss = .);
    *(.sbss)
    *(.sbss*)
    *(.bss)
    *(.bss*) 
    *(COMMON*)
    . = ALIGN(4);
    PROVIDE(_ebss = .);
  } > RAM

  PROVIDE(_end = _ebss);
  PROVIDE(end = . );
  PROVIDE(_eusrstack = ORIGIN(RAM) + LENGTH(RAM));	
}
