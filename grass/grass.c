/*
 * (C) 2023, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a demo program of timer interrupt
 */


#include "egos.h"

int timer_reset();
struct earth *earth = (void*)GRASS_STACK_TOP;

void handler() {
    earth->tty_info("Got timer interrupt.");
    timer_reset();  // see timer.c
}

int main() {
    earth->tty_success("A timer interrupt demo.");

    asm("csrw mtvec, %0" ::"r"(handler));
    timer_reset();  // see timer.c

    int mstatus, mie;
    asm("csrr %0, mstatus" : "=r"(mstatus));
    asm("csrw mstatus, %0" ::"r"(mstatus | 0x8));
    asm("csrr %0, mie" : "=r"(mie));
    asm("csrw mie, %0" ::"r"(mie | 0x80));
    
    while(1);
}
