/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: Kernel ≈ 3 handlers
 *     proc_yield() handles timer interrupt for process scheduling
 *     excp_entry() handles faults such as unauthorized memory access
 *     proc_syscall() handles system calls for inter-process communication
 */


#include "egos.h"
#include "process.h"
#include "syscall.h"
#include <string.h>

void kernel_entry(uint is_interrupt, uint id) {
    /* Save process context */
    asm("csrr %0, mepc" : "=r"(proc_set[proc_curr_idx].mepc));
    memcpy(proc_set[proc_curr_idx].saved_register, SAVED_REGISTER_ADDR, SAVED_REGISTER_SIZE);

    (is_interrupt)? intr_entry(id) : excp_entry(id);

    /* Restore process context */
    asm("csrw mepc, %0" ::"r"(proc_set[proc_curr_idx].mepc));
    memcpy(SAVED_REGISTER_ADDR, proc_set[proc_curr_idx].saved_register, SAVED_REGISTER_SIZE);
}

#define EXCP_ID_ECALL_U    8
#define EXCP_ID_ECALL_M    11

void excp_entry(uint id) {
    /* Student's code goes here (system call and memory exception). */

    /* If id is for system call, handle the system call and return */

    /* Otherwise, kill the process if curr_pid is a user application */

    /* Student's code ends here. */
    FATAL("excp_entry: kernel got exception %d", id);
}

#define INTR_ID_SOFT       3
#define INTR_ID_TIMER      7

static void proc_yield();
static void proc_syscall();

uint proc_curr_idx;
struct process proc_set[MAX_NPROCESS];

void intr_entry(uint id) {
    if (id == INTR_ID_TIMER && curr_pid < GPID_SHELL) {
        /* Do not interrupt kernel processes since IO can be stateful */
        earth->timer_reset();
        return;
    }

    if (earth->tty_recv_intr() && curr_pid >= GPID_USER_START) {
        /* User process killed by ctrl+c interrupt */
        INFO("process %d killed by interrupt", curr_pid);
        asm("csrw mepc, %0" ::"r"(0x800500C));
        return;
    }

    /* Ignore other interrupts for now */
    if (id == INTR_ID_SOFT) proc_syscall();
    if (id == INTR_ID_TIMER) proc_yield();
}

static void proc_yield() {
    /* Find the next runnable process */
    int next_idx = -1;
    for (uint i = 1; i <= MAX_NPROCESS; i++) {
        enum proc_status s = proc_set[(proc_curr_idx + i) % MAX_NPROCESS].status;
        if (s == PROC_READY || s == PROC_RUNNING || s == PROC_RUNNABLE) {
            next_idx = (proc_curr_idx + i) % MAX_NPROCESS;
            break;
        }
    }

    if (next_idx == -1) FATAL("proc_yield: no runnable process");
    if (curr_status == PROC_RUNNING) proc_set_runnable(curr_pid);

    /* Switch to the next runnable process and reset timer */
    proc_curr_idx = next_idx;
    earth->mmu_switch(curr_pid);
    earth->timer_reset();

    /* Student's code goes here (switch privilege level). */

    /* Modify mstatus.MPP to enter machine or user mode during mret
     * depending on whether curr_pid is a grass server or a user app
     */

    /* Student's code ends here. */

    /* Call the entry point for newly created process */
    if (curr_status == PROC_READY) {
        /* Set argc, argv and initial program counter */
        proc_set[proc_curr_idx].saved_register[8] = APPS_ARG;
        proc_set[proc_curr_idx].saved_register[9] = APPS_ARG + 4;
        proc_set[proc_curr_idx].mepc = APPS_ENTRY;
    }

    proc_set_running(curr_pid);
}

static void proc_send(struct syscall *sc) {
    sc->msg.sender = curr_pid;
    int receiver = sc->msg.receiver;

    for (uint i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid == receiver) {
            /* Find the receiver */
            if (proc_set[i].status != PROC_WAIT_TO_RECV) {
                curr_status = PROC_WAIT_TO_SEND;
                proc_set[proc_curr_idx].receiver_pid = receiver;
            } else {
                /* Copy message from sender to kernel stack */
                struct sys_msg tmp;
                earth->mmu_switch(curr_pid);
                memcpy(&tmp, &sc->msg, sizeof(tmp));

                /* Copy message from kernel stack to receiver */
                earth->mmu_switch(receiver);
                memcpy(&sc->msg, &tmp, sizeof(tmp));

                /* Set receiver process as runnable */
                proc_set_runnable(receiver);
            }
            proc_yield();
            return;
        }

    sc->retval = -1;
}

static void proc_recv(struct syscall *sc) {
    int sender = -1;
    for (uint i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].status == PROC_WAIT_TO_SEND &&
            proc_set[i].receiver_pid == curr_pid)
            sender = proc_set[i].pid;

    if (sender == -1) {
        curr_status = PROC_WAIT_TO_RECV;
    } else {
        /* Copy message from sender to kernel stack */
        struct sys_msg tmp;
        earth->mmu_switch(sender);
        memcpy(&tmp, &sc->msg, sizeof(tmp));

        /* Copy message from kernel stack to receiver */
        earth->mmu_switch(curr_pid);
        memcpy(&sc->msg, &tmp, sizeof(tmp));

        /* Set sender process as runnable */
        proc_set_runnable(sender);
    }

    proc_yield();
}

static void proc_syscall() {
    struct syscall *sc = (struct syscall*)SYSCALL_ARG;

    enum syscall_type type = sc->type;
    sc->retval = 0;
    sc->type = SYS_UNUSED;
    *((int*)MSIP) = 0;

    switch (type) {
    case SYS_RECV:
        proc_recv(sc);
        break;
    case SYS_SEND:
        proc_send(sc);
        break;
    default:
        FATAL("proc_syscall: got unknown syscall type=%d", type);
    }
}
