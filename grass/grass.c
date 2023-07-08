/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: grass layer initialization
 * Initialize timer and the process control block; 
 * Spawn the first kernel process, GPID_PROCESS (pid=1).
 */

#include "egos.h"
#include "process.h"
#include "syscall.h"

struct grass *grass = (void*)APPS_STACK_TOP;
struct earth *earth = (void*)GRASS_STACK_TOP;

static int sys_proc_read(int block_no, int* dst) {
    return earth->disk_read(SYS_PROC_EXEC_START + block_no, 1, dst);
}

extern void (*apps_entry[16])();
int main() {
    CRITICAL(L"Enter the grass layer");

    /* Initialize the grass interface functions */
    grass->proc_alloc = proc_alloc;
    grass->proc_free = proc_free;
    grass->proc_entry = proc_entry;
    grass->proc_set_ready = proc_set_ready;

    grass->sys_exit = sys_exit;
    grass->sys_send = sys_send;
    grass->sys_recv = sys_recv;
    grass->tmp_ecall = intr_entry;
    
    /* Load and enter the first kernel process sys_proc */
    INFO(L"Load kernel process #%d: sys_proc", GPID_PROCESS);
    elf_load(GPID_PROCESS, sys_proc_read, 0, 0);
    earth->mmu_switch(GPID_PROCESS);

    proc_init();
    /* egos-2000 could enter the shell without timer interrupt */
    //timer_init();
    //timer_reset();
    //if (earth->translation == SOFT_TLB) earth->intr_enable();

    int stack_top = (int)apps_entry[GPID_PROCESS] + PAGE_SIZE * 5;
    asm("mv a0, %0" ::"r"(APPS_ARG));
    asm("mv t0, %0" ::"r"(stack_top));
    apps_entry[GPID_PROCESS]();
}
