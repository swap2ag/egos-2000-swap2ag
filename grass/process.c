/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: helper functions for managing processes
 */

#include "egos.h"
#include "process.h"
#include "syscall.h"

void intr_entry(int id);

void excp_entry(int id) {
    /* Student's code goes here (handle memory exception). */

    /* If id is for system call, handle the system call and return */

    /* Otherwise, kill the process if curr_pid is a user application */

    /* Student's code ends here. */

    FATAL(L"excp_entry: kernel got exception %d", id);
}

static int proc_nprocs;
void proc_init() {
    earth->intr_register(intr_entry);
    earth->excp_register(excp_entry);

    /* Student's code goes here (PMP memory protection). */

    /* Setup PMP TOR region 0x00000000 - 0x20000000 as r/w/x */

    /* Setup PMP NAPOT region 0x20400000 - 0x20800000 as r/-/x */

    /* Setup PMP NAPOT region 0x20800000 - 0x20C00000 as r/-/- */

    /* Setup PMP NAPOT region 0x80000000 - 0x80004000 as r/w/- */

    /* Student's code ends here. */

    /* The first process is currently running */
    proc_nprocs = 0;
    proc_curr_idx = 0;
    for (int i = 0; i < MAX_NPROCESS; i++) {
        proc_set[i].pid = 0;
        proc_set[i].status = PROC_UNUSED;
    }
    proc_set_running(proc_alloc());
}

static void proc_set_status(int pid, int status) {
    for (int i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid == pid) proc_set[i].status = status;
}

int proc_alloc() {
    for (int i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].status == PROC_UNUSED) {
            proc_set[i].pid = ++proc_nprocs;
            proc_set[i].status = PROC_LOADING;
            return proc_nprocs;
        }

    FATAL(L"proc_alloc: reach the limit of %d processes", MAX_NPROCESS);
}

void proc_free(int pid) {
    if (pid != -1) {
        //earth->mmu_free(pid);
        proc_set_status(pid, PROC_UNUSED);
        return;
    }

    /* Free all user applications */
    for (int i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid >= GPID_USER_START &&
            proc_set[i].status != PROC_UNUSED) {
            //earth->mmu_free(proc_set[i].pid);
            proc_set[i].status = PROC_UNUSED;
        }
}

int proc_entry(int pid) {
    if (pid == -1) pid = curr_pid;
    int apps_entry[] = {0, 0x00521000, 0x00541000, 0x00561000, 0x00581000};
    return (pid < GPID_USER_START)? apps_entry[pid] : 0x00200000;
}

void proc_set_ready(int pid) { proc_set_status(pid, PROC_READY); }
void proc_set_running(int pid) { proc_set_status(pid, PROC_RUNNING); }
void proc_set_runnable(int pid) { proc_set_status(pid, PROC_RUNNABLE); }
