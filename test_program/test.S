        .equ STDOUT, 1
        .equ WRITE, 64
        .equ EXIT, 93

        .section .rodata
        .align 2
msg:
        .ascii "RISC-V\n"

        .section .text
        .align 2

        .globl  _start
_start:

        li a0, STDOUT  # file descriptor
        la a1, msg     # address of string
        li a2, 7       # length of string
        li a7, WRITE   # syscall number for write
        ecall

        # MISSING: Check for error condition

        li a0, 0       # 0 signals success
        li a7, EXIT
        ecall
