ENTRY(_reset);

/* Memory as specified in the datasheet*/
MEMORY {
  flash(rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  sram(rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}


/* The stack pointer points to the top(end) of the stack */
_stack_pointer = ORIGIN(sram) + LENGTH(sram);

SECTIONS {
  .vectors  : { KEEP(*(.vectors)) }   > flash
  .text     : { *(.text*) }           > flash
  .rodata   : { *(.rodata*) }         > flash

  .data : {
    _sdata = .;                      
    *(.data)                         
    *(.data*)                         
    _edata = .;                       
  } > sram AT > flash
  _sidata = LOADADDR(.data);

  .bss : {
      _sbss = .;
      *(.bss)
      *(COMMON)
      _ebss = .;
  } > sram
  
  _end = .;  
  
}