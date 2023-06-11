/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a 1MB (256*4KB) paging device
 * for QEMU, 256 physical frames start at address FRAME_CACHE_START
 * for Arty, 28 physical frames are cached at address FRAME_CACHE_START
 * and 256 frames (1MB) start at the beginning of the microSD card
 */

#include "egos.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#define ARTY_CACHED_NFRAMES 28
#define NBLOCKS_PER_PAGE PAGE_SIZE / BLOCK_SIZE  /* 4KB / 512B == 8 */

int cache_slots[ARTY_CACHED_NFRAMES];
char *pages_start = (void*)FRAME_CACHE_START;

void paging_init() { memset(cache_slots, 0xFF, sizeof(cache_slots)); }

int paging_write(int frame_id, int page_no) {
    char* src = (void*)(page_no << 12);
    memcpy(pages_start + frame_id * PAGE_SIZE, src, PAGE_SIZE);
    return 0;
}

char* paging_read(int frame_id, int alloc_only) {
    return pages_start + frame_id * PAGE_SIZE;
}
