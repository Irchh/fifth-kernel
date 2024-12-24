ENTRY(_start);
 
. = 0x80000000;
 
SECTIONS {
	/* Include entry point at start of binary */
    kernel_start_ptr = .;
	.text : ALIGN(4K) {
		*(.init);
		*(.text);
	}
	.bss : ALIGN(4K) {
		PROVIDE(bss_start = .);
		*(.bss);
		. += 4096;
		PROVIDE(stack_top = .);
		. += 4096;
		PROVIDE(global_pointer = .);
		PROVIDE(bss_end = .);
	}
	.rodata : ALIGN(4K) {
	    PROVIDE(rodata_start = .);
		*(.rodata);
		PROVIDE(rodata_middle = .);
		*(.rodata.*);
		PROVIDE(rodata_end = .);
	}
	.data : ALIGN(4K) {
		*(.data);
		*(.data.*);
	}
	. = ALIGN(4K);
    kernel_end_ptr = .;
}
