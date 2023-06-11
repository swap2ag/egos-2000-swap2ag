/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple echo
 */

#include "app.h"

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) printf(L"%s ", argv[i]);
    printf(L"\r\n");
    return 0;
}
