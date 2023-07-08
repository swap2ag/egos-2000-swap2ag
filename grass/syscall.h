#pragma once

#include "servers.h"

enum syscall_type {
	SYS_UNUSED,
	SYS_RECV,
	SYS_SEND,
	SYS_NCALLS
};

struct sys_msg {
    int sender;
    int receiver;
    int content[SYSCALL_MSG_LEN / sizeof(int)];
};

struct syscall {
    enum syscall_type type;  /* Type of the system call */
    struct sys_msg msg;      /* Data of the system call */
    int retval;              /* Return value of the system call */
};

void sys_exit(int status);
int  sys_send(int pid, int* msg, int size);
int  sys_recv(int* pid, int* buf, int size);
void  intr_entry(int id);
