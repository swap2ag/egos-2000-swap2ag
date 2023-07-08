/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple cat
 */

#include "app.h"
extern int my_strlen(int* src);

int main(int argc, wchar_t ** argv) {
    if (argc == 1) {
        INFO(L"usage: cat [FILE]");
        return -1;
    }

    /* Get the inode number of the file */
    int file_ino = dir_lookup(grass->workdir_ino, argv[1]);
    if (file_ino < 0) {
        INFO(L"cat: file %s not found", argv[1]);
        return -1;
    }

    /* Read and print the first block of the file */
    int buf[BLOCK_SIZE / sizeof(int)];
    file_read(file_ino, 0, buf);
    printf(L"%s", buf);
    if (buf[strlen(buf) - 1] != '\n') printf(L"\r\n");

    return 0;
}
