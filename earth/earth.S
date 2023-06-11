/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: boot loader
 * i.e., the first instructions executed by the CPU when boot up
 */
    .section .image.placeholder
    .section .text.enter
    .global _enter
_enter:
    /* Initialize global pointer */
    auipc gp, %pcrel_hi(_gp)
    addi  gp, gp, %pcrel_lo(_enter)

    /* Call main() of earth.c */
    li sp, 0x1FF000
    call main
