.section .init
.global _start

_start:
.option push
.option norelax
    la gp, global_pointer
.option pop
    csrw satp, zero
    la sp, stack_top

    la t5, bss_start
    la t6, bss_end
bss_clear:
    sd zero, (t5)
    addi t5, t5, 8
    bltu t5, t6, bss_clear

    la t0, kernel_main
    csrw mepc, t0

    tail kernel_main
