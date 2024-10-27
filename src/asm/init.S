.section .init
.global _start

_start:
.option push
.option norelax
    la gp, global_pointer
.option pop
    csrw satp, zero
    // Go to infinite loop if there are more than 32 harts
    csrr t1, mhartid
    addi t0, zero, 32
    bgeu t1, t0, loop

    add t0, zero, t1

    la sp, stack_top

    la t5, bss_start
    la t6, bss_end
bss_clear:
    sd zero, (t5)
    addi t5, t5, 8
    bltu t5, t6, bss_clear

    la t0, kernel_init
    csrw mepc, t0

    tail kernel_init

loop:
    j loop
