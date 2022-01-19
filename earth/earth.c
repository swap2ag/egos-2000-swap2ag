/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: initialize dev_tty, dev_disk, cpu_intr and cpu_mmu;
 * load the grass layer binary from disk and run it
 */


#include "egos.h"
#include "earth.h"

static struct earth earth;

int main() {
    INFO("Start to initialize the earth layer");

    if (tty_init()) {
        ERROR("Failed at initializing the tty device");
        return -1;
    }
    earth.tty_read = tty_read;
    earth.tty_write = tty_write;
    SUCCESS("Finished initializing the tty device");
    
    if (disk_init()) {
        ERROR("Failed at initializing the disk device");
        return -1;
    }
    earth.disk_read = disk_read;
    earth.disk_write = disk_write;
    SUCCESS("Finished initializing the disk device");

    if (intr_init()) {
        ERROR("Failed at initializing the CPU interrupts");
        return -1;
    }
    earth.intr_enable = intr_enable;
    earth.intr_disable = intr_disable;
    SUCCESS("Finished initializing the CPU interrupts");
    

    return 0;
}