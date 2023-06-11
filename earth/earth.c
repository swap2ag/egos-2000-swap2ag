/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: Initialize the bss and data segments;
 * Initialize dev_tty, dev_disk, cpu_intr and cpu_mmu;
 * Load the grass layer binary from disk and run it.
 */

#include "elf.h"
#include "disk.h"
#include "egos.h"
#include <string.h>

void tty_init();
void disk_init();
void intr_init();
void mmu_init();

struct grass *grass = (void*)APPS_STACK_TOP;
struct earth *earth = (void*)GRASS_STACK_TOP;

static void earth_init() {
    tty_init();
    CRITICAL(L"------------- Booting -------------");
    SUCCESS(L"Finished initializing the tty device");

    intr_init();
    SUCCESS(L"Finished initializing the CPU interrupts");

    disk_init();
    SUCCESS(L"Finished initializing the disk device");

    mmu_init();
    SUCCESS(L"Finished initializing the CPU memory management unit");
    earth->platform = ECE4750;
}

static int grass_read(int block_no, char* dst) {
    return earth->disk_read(GRASS_EXEC_START + block_no, 1, dst);
}

int main() {
    /* Initialize the earth layer */
    earth_init();

    /* Load and enter the grass layer */
    elf_load(0, grass_read, 0, 0);
    void (*grass_entry)() = (void*)GRASS_ENTRY;
    grass_entry();
}
