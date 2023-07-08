/*
 * (C) 2023, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: memcpy and memset without byte access
 */

#include "egos.h"

void my_memcpy(void* dst, void* src, int len) {
    if (len % 4 != 0)
        FATAL(L"my_memcpy can only copy words");
    for (int i = 0; i < len / 4; i++)
        ((int*)dst)[i] = ((int*)src)[i];
}

void my_memset(void* dst, int val, int len) {
    if (len % 4 != 0)
        FATAL(L"my_memset can only copy words");

    if (val != 0 && val != 0xFF)
        FATAL(L"my_memset only supports 0 or 0xFF");
    if (val == 0xFF) val = -1;

    for (int i = 0; i < len / 4; i++)
        ((int*)dst)[i] = val;
}

int my_strcmp(int* left, int* right) {
    int idx = 0;
    while (1) {
        if (left[idx] == right[idx]) {
            if (left[idx] == 0) return 0;
            idx++;
            continue;
        }
        return -1;
    }
}

int my_strcpy(int* dst, int* src) {
    int idx = 0;
    while (1) {
        dst[idx] = src[idx];
        if (src[idx] == 0)
            return 0;
        idx++;
    }
}

int my_strlen(int* src) {
    int idx = 0;
    while (1) {
        if (src[idx] == 0)
            return idx;
        idx++;
    }
}