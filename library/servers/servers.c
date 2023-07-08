/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: user friendly interfaces of system processes
 * including GPID_PROCESS, GPID_FILE and GPID_DIR
 */

#include "egos.h"
#include "servers.h"
extern int my_strcpy(int* dst, int* src);
extern void my_memcpy(void* dst, void* src, int len);

static int sender;
static int buf[SYSCALL_MSG_LEN / sizeof(int)];

void exit(int status) {
    grass->sys_exit(status);
    while(1);
}

int dir_lookup(int dir_ino, int* name) {
    struct dir_request req;
    req.type = DIR_LOOKUP;
    req.ino = dir_ino;
    strcpy(req.name, name);
    grass->sys_send(GPID_DIR, (void*)&req, sizeof(req));

    grass->sys_recv(&sender, buf, sizeof(struct dir_reply));
    if (sender != GPID_DIR) FATAL(L"dir_lookup: an error occurred");
    struct dir_reply *reply = (void*)buf;

    return reply->status == DIR_OK? reply->ino : -1;
}

int file_read(int file_ino, int offset, int* block) {
    struct file_request req;
    req.type = FILE_READ;
    req.ino = file_ino;
    req.offset = offset;
    grass->sys_send(GPID_FILE, (void*)&req, sizeof(req));

    grass->sys_recv(&sender, buf, sizeof(struct file_reply));
    if (sender != GPID_FILE) FATAL(L"file_read: an error occurred");
    struct file_reply *reply = (void*)buf;
    memcpy(block, reply->block.bytes, BLOCK_SIZE);

    return reply->status == FILE_OK? 0 : -1;
}
