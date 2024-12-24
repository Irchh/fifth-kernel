.global setup_post_relocation

.macro relocate_reg reg1 reg2
    ld \reg1, kernel_start_addr
    bltu \reg2, \reg1, _do_nothing_\reg2
    ld t0, kernel_end_addr
    bgeu \reg2, \reg1, _do_nothing_\reg2
_relocate_\reg2:
    add \reg2, \reg2, a0
_do_nothing_\reg2:
.endm

setup_post_relocation:
    csrrw sp, sscratch, sp
    add sp, sp, a0
    csrrw sp, sscratch, sp

    addi sp, sp, -0x20

    sd t0, 0x8(sp)
    relocate_reg t0, sp
    relocate_reg t0, gp
    add fp, fp, a0
    relocate_reg t0, a1
    relocate_reg t0, a2
    relocate_reg t0, a3
    relocate_reg t0, a4
    relocate_reg t0, a5
    relocate_reg t0, a6
    relocate_reg t0, a7
    relocate_reg t0, t1
    relocate_reg t0, t2
    relocate_reg t0, t3
    relocate_reg t0, t4
    relocate_reg t0, t5
    relocate_reg t0, t6
    relocate_reg t0, s1
    add s2, s2, a0
    relocate_reg t0, s3
    relocate_reg t0, s4
    relocate_reg t0, s5
    relocate_reg t0, s6
    relocate_reg t0, s7
    relocate_reg t0, s8
    relocate_reg t0, s9
    relocate_reg t0, s10
    relocate_reg t0, s11
    ld t0, 0x8(sp)

    sd t1, 0x8(sp)
    add t0, t0, a0
    ld t1, 0x8(sp)

    addi sp, sp, 0x20
    call modify_rodata_ptrs
    call kernel_main
    ret