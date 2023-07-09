/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: load an ELF-format executable file into memory
 * Only use the program header instead of the multiple section headers.
 */

#include "egos.h"
#include "elf.h"
#include "disk.h"
#include "servers.h"

#include <string.h>

void (*grass_entry)();
static void load_grass(elf_reader reader,
                       struct elf32_program_header* pheader) {
    INFO(L"Grass kernel file size: %d bytes", pheader->p_filesz);
    INFO(L"Grass kernel memory size: %d bytes", pheader->p_memsz);

    grass_entry = (void*)(GRASS_START + pheader->p_offset);
}

void (*apps_entry[16])();
static void load_app(int pid, elf_reader reader,
                     int argc, void** argv,
                     struct elf32_program_header* pheader) {

    /* Debug printing during bootup */
    if (pid < GPID_USER_START) {
        INFO(L"App file size: %d bytes", pheader->p_filesz);
        INFO(L"App memory size: %d bytes", pheader->p_memsz);

        apps_entry[pid] = (void*)(GRASS_START + GRASS_EXEC_NBYTE * pid + pheader->p_offset);
    } else {
        apps_entry[pid] = (void*)(APPS_ENTRY);
        INFO(L"Start loading app binary: %d bytes", pheader->p_filesz);

        int block_offset = pheader->p_offset / BLOCK_SIZE;
        for (int off = 0; off < pheader->p_filesz; off += BLOCK_SIZE) {
            reader(block_offset++, (void*)(APPS_ENTRY + off));
        }
        INFO(L"Finish loading app binary");

        *((int*)APPS_ARG) = argc;
        memcpy((void*)(APPS_ARG + 4 + 4 * argc), argv, 16 * argc * 4);
        for (int i = 0; i < argc; i++) {
            int* argv_i = (void*) (APPS_ARG + 4 + 4 * i);
            *argv_i = APPS_ARG + 4 + 4 * argc + 16 * i * 4;
        }
        INFO(L"Finish loading argc and argv");
    }
}

void elf_load(int pid, elf_reader reader, int argc, void** argv) {
    char buf[BLOCK_SIZE];
    reader(0, (void*)buf);

    struct elf32_header *header = (void*) buf;
    struct elf32_program_header *pheader = (void*)(buf + header->e_phoff);

    if (pheader->p_vaddr == GRASS_ENTRY)
        load_grass(reader, pheader);
    else
        load_app(pid, reader, argc, argv, pheader);
}