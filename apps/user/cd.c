/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple cd
 */

#include "app.h"
int my_strcpy(int* dst, int* src);

int main(int argc, char** argv) {
    INFO(L"within cd");
    if (argc == 1) {
        int home_ino = dir_lookup(0, L"home/");
        grass->workdir_ino = dir_lookup(home_ino, L"yunhao/");
        strcpy(grass->workdir, L"/home/yunhao");
        return 0;
    }
    FATAL(L"cd: not implemented");

    /* Set the inode number to the new working directory
    strcat(argv[1], "/");
    int dir_ino = dir_lookup(grass->workdir_ino, argv[1]);
    if (dir_ino == -1) {
        INFO(L"cd: directory %s not found", argv[1]);
        return -1;
    }
    grass->workdir_ino = dir_ino; */

    /* Set the path name to the new working directory
    if (!strcmp("./", argv[1])) return 0;

    int len = strlen(grass->workdir);
    if (strcmp("../", argv[1])) {
        if (len > 1) strcat(grass->workdir, "/");
        strncat(grass->workdir, argv[1], strlen(argv[1]) - 1);
    } else {
        while (grass->workdir[len] != '/') grass->workdir[len--] = 0;
        if (len) grass->workdir[len] = 0;
    } */
    
    return 0;
}
