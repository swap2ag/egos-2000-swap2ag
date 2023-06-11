/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple disk device driver
 */

#include "egos.h"
#include "disk.h"
#include "bus_gpio.c"
#include <string.h>

enum {
      SD_CARD,
      FLASH_ROM
};
static int type;

int disk_read(int block_no, int nblocks, char* dst) {
    char* src = (char*)0x00400000 + block_no * BLOCK_SIZE;
    memcpy(dst, src, nblocks * BLOCK_SIZE);
    return 0;
}

int disk_write(int block_no, int nblocks, char* src) {
    FATAL("Attempt to write the on-board ROM");
}

void disk_init() {
    earth->disk_read = disk_read;
    earth->disk_write = disk_write;

    type = FLASH_ROM;
    return;
}
