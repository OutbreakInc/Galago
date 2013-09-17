MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 32k
	SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 8k
	/*USBSRAM (rwx) : ORIGIN = 0x20080000, LENGTH = 16k*/
}

SECTIONS
{
	.text :
	{
		PROVIDE(stext = .);
		KEEP(*(.isr_vector .isr_vector.*))
		*(.startup .startup.*)

		. = ALIGN(4);
		PROVIDE(__init_start = .);
		KEEP(*(SORT(.init_array)))
		PROVIDE(__init_end = .);

		*(.text .text.*)
		*(.gnu.linkonce.t.*)
		*(.glue_7)
		*(.glue_7t)
		*(.gcc_except_table)
		*(.rodata .rodata*)
		*(.gnu.linkonce.r.*)
		. = ALIGN(4);
	} >FLASH
	.ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >FLASH
	PROVIDE_HIDDEN (__exidx_end = .);

	.ctors :
	{
		. = ALIGN(4);
		PROVIDE(__ctors_start = .);
		KEEP(*(SORT(.ctors.*)))
		KEEP(*(.ctors))
		PROVIDE(__ctors_end = .);
	} >FLASH

	.dtors :
	{
		. = ALIGN(4);
		PROVIDE(__dtors_start = .);
		KEEP(*(SORT(.dtors.*)))
		KEEP(*(.dtors))
		PROVIDE(__dtors_end = .);

		. = ALIGN(4);
		/* End of .text section */
		etext = .;
		_fini = .;
		_sidata = .;
	} >FLASH

	/*.usb_ram (NOLOAD):
	{
		*.o (USB_RAM)
	} > USBSRAM*/

	.data : AT (_sidata)
  	{
  		. = ALIGN(4);
		_sdata = .;

		*(vtable vtable.*)
		*(.data .data.*)
		*(.gnu.linkonce.d*)

		. = ALIGN(4);
		_edata = . ;
	} >SRAM

	__bss_start = .;
	__bss_start__ = .;
	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		_sbss = . ;

		*(.bss .bss.*)
		*(.gnu.linkonce.b*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = . ;
	} >SRAM
	_bss_end__ = . ;
	__bss_end__ = . ;
	__end__ = . ;
	PROVIDE(__heap_start__ = .);
	
	. += (8k - 512);
	
	PROVIDE(__heap_end__ = .);

  	.stackarea (NOLOAD) :
  	{
	    . = ALIGN(8);
	    _sstack = .;

		*(.stackarea .stackarea.*)

		. = ALIGN(8);
		_estack = .;

		. = ALIGN(4);
		_end = . ;
		PROVIDE(end = .);

  	} > SRAM

	/* Debugging */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	/* .comment       0 : { *(.comment) } */
	/* DWARF debug sections.
	 Symbols in the DWARF debugging sections are relative to the beginning
	 of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
}
