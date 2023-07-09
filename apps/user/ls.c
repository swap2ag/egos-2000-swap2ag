/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple ls
 */

#include "app.h"
extern int my_strlen(int* src);

int main(int argc, wchar_t ** argv) {
    INFO(L"Enter the main() of ls");

    if (argc > 1) {
        INFO(L"ls: ls with args not implemented");
        return -1;
    }

    /* Read the directory content */
    int buf[BLOCK_SIZE / sizeof(int)];
    file_read(grass->workdir_ino, 0, buf);

    /* Remove the inode numbers from the string */
    for (int i = 1; i < strlen(buf); i++)
        if (buf[i - 1] == ' ' && buf[i] >= '0' && buf[i] <= '9') buf[i] = ' ';

    /* Print the directory content */
    printf(L"%s\r\n", buf);
    return 0;
}
