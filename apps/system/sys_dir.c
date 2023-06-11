/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: the directory system server
 * handling requests to directory lookup, insertion, etc.
 */

#include "app.h"
#include <stdlib.h>
extern void my_memcpy(void* dst, void* src, int len);

/* To understand directory management, read tools/mkfs.c */
int dir_do_lookup(int dir_ino, char* name) {
    FATAL(L"dir_do_lookup: not implemented");
    /*
    char buf[BLOCK_SIZE];
    file_read(dir_ino, 0, buf);

    for (int i = 0, namelen = strlen(name); i < strlen(buf) - namelen; i++)
        if (!strncmp(name, buf + i, namelen) &&
            buf[i + namelen] == ' ' && (i == 0 || buf[i - 1] == ' '))
            return atoi(buf + i + namelen);
    return -1;
    */
}

int main() {
    SUCCESS(L"Enter kernel process GPID_DIR");

    /* Send a notification to GPID_PROCESS */
    char buf[SYSCALL_MSG_LEN];
    memcpy(buf, L"Finish GPID_DIR initialization", 31 * 4);
    grass->sys_send(GPID_PROCESS, buf, 31 * 4);

    /* Wait for directory requests */
    while (1) {
        int sender;
        struct dir_request *req = (void*)buf;
        struct dir_reply *reply = (void*)buf;
        grass->sys_recv(&sender, buf, SYSCALL_MSG_LEN);

        switch (req->type) {
        case DIR_LOOKUP:
            reply->ino = dir_do_lookup(req->ino, req->name);
            reply->status = reply->ino == -1? DIR_ERROR : DIR_OK;
            grass->sys_send(sender, (void*)reply, sizeof(*reply));
            break;
        case DIR_INSERT: case DIR_REMOVE: default:
            /* This part is left to students as an exercise */
            FATAL(L"sys_dir: request%d not implemented", req->type);
        }
    }
}
