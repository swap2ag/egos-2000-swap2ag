/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: system process for spawning and killing other processes
 */

#include "elf.h"
#include "app.h"
#include "disk.h"

static int app_ino, app_pid;
static void sys_spawn(int base);
static int app_spawn(struct proc_request *req);

int main() {
    SUCCESS(L"Enter kernel process GPID_PROCESS");

    int sender, shell_waiting;
    char buf[SYSCALL_MSG_LEN];

    sys_spawn(SYS_FILE_EXEC_START);
    grass->sys_recv(NULL, buf, SYSCALL_MSG_LEN);
    INFO(L"sys_proc receives: %s", buf);

    sys_spawn(SYS_DIR_EXEC_START);
    grass->sys_recv(NULL, buf, SYSCALL_MSG_LEN);
    INFO(L"sys_proc receives: %s", buf);

    sys_spawn(SYS_SHELL_EXEC_START);
    
    while (1) {
        struct proc_request *req = (void*)buf;
        struct proc_reply *reply = (void*)buf;
        grass->sys_recv(&sender, buf, SYSCALL_MSG_LEN);

        switch (req->type) {
        case PROC_SPAWN:
            reply->type = app_spawn(req) < 0 ? CMD_ERROR : CMD_OK;

            /* Handling background processes */
            shell_waiting = 1;
            //shell_waiting = (req->argv[req->argc - 1][0] != '&');
            if (!shell_waiting && app_pid > 0)
                INFO(L"process %d running in the background", app_pid);
            grass->sys_send(GPID_SHELL, (void*)reply, sizeof(reply));
            break;
        case PROC_EXIT:
            grass->proc_free(sender);

            if (shell_waiting && app_pid == sender)
                grass->sys_send(GPID_SHELL, (void*)reply, sizeof(reply));
            else
                INFO(L"background process %d terminated", sender);
            break;
        case PROC_KILLALL:
            grass->proc_free(-1); break;
        default:
            FATAL(L"sys_proc: invalid request %d", req->type);
        }
    }
}

static int app_read(int off, char* dst) { file_read(app_ino, off, dst); }

static int app_spawn(struct proc_request *req) {
    /*
    int bin_ino = dir_lookup(0, "bin/");
    if ((app_ino = dir_lookup(bin_ino, req->argv[0])) < 0) return -1;

    app_pid = grass->proc_alloc();

    elf_load(app_pid, app_read, req->argc, (void**)req->argv);
    grass->proc_set_ready(app_pid);
    */
    return 0;
}

static int sys_proc_base;
wchar_t* sysproc_names[] = {L"sys_proc", L"sys_file", L"sys_dir", L"sys_shell"};

static int sys_proc_read(int block_no, char* dst) {
    return earth->disk_read(sys_proc_base + block_no, 1, dst);
}

static void sys_spawn(int base) {
    INFO(L"Allocate process");
    int pid = grass->proc_alloc();
    INFO(L"Allocated process %d", pid);
    INFO(L"Load kernel process #%d: %s", pid, sysproc_names[pid - 1]);

    sys_proc_base = base;
    elf_load(pid, sys_proc_read, 0, NULL);
    grass->proc_set_ready(pid);
}
