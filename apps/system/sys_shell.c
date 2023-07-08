/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple shell
 */

#include "app.h"

extern int my_strlen(int* src);
extern int my_strcpy(int* dst, int* src);
extern int my_strcmp(int* left, int* right);
extern void my_memset(void* dst, int val, int len);
extern void my_memcpy(void* dst, void* src, int len);

int parse_request(wchar_t* buf, struct proc_request* req) {
    int idx = 0, nargs = 0;
    memset(req->argv, 0, CMD_NARGS * CMD_ARG_LEN);

    for (int i = 0; i < strlen(buf); i++)
        if (buf[i] != ' ') {
            req->argv[nargs][idx] = buf[i];
            if (++idx >= CMD_ARG_LEN) return -1;
        } else if (idx != 0) {
            idx = req->argv[nargs][idx] = 0;
            if (++nargs >= CMD_NARGS) return -1;
        }
    req->argc = idx ? nargs + 1 : nargs;
    return 0;
}

int main() {
    CRITICAL(L"Welcome to the egos-2000 shell!");

    /* Enter the home directory first.
     * By default, the inode number of /home is 1
     * and the inode number of /home/yunhao is 2.
     * The code below skips the dir_lookup() functions for 
     * better performance, but dir_lookup() indeed works.
     * It is just too slow in the simulation.
     */

    int home_ino = 1;
    //int home_ino = dir_lookup(0, L"home/");
    grass->workdir_ino = 2;
    //grass->workdir_ino = dir_lookup(home_ino, L"yunhao/");
    strcpy(grass->workdir, L"/home/yunhao");

    int buf[64];
    while (1) {
        do {
            printf(L"\x1B[1;32m➜ \x1B[1;36m%s\x1B[1;0m ", grass->workdir);
        } while (earth->tty_read(buf, 64) == 0);
        //INFO(L"execute command: %s", buf);

        struct proc_request req;
        struct proc_reply reply;

        if (strcmp(buf, L"killall") == 0) {
            req.type = PROC_KILLALL;
            grass->sys_send(GPID_PROCESS, (void*)&req, sizeof(req));
        } else {
            req.type = PROC_SPAWN;

            if (0 != parse_request(buf, &req)) {
                INFO(L"sys_shell: too many arguments or argument too long");
            } else {
                //INFO(L"shell: argc=%d, cmd=%s", req.argc, req.argv[0]);
                grass->sys_send(GPID_PROCESS, (void*)&req, sizeof(req));
                grass->sys_recv(NULL, (void*)&reply, sizeof(reply));

                if (reply.type != CMD_OK)
                    INFO(L"sys_shell: command causes an error");
                else if (req.argv[req.argc - 1][0] != '&')
                    /* Wait for foreground command to terminate */
                    grass->sys_recv(NULL, (void*)&reply, sizeof(reply));
            }
        }
    }
}
