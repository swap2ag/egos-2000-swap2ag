#pragma once
#include <wchar.h>

struct earth {
    /* CPU interface */
    int (*intr_enable)();
    int (*intr_register)(void (*handler)(int));
    int (*excp_register)(void (*handler)(int));

    int (*mmu_alloc)(int* frame_no, void** cached_addr);
    int (*mmu_free)(int pid);
    int (*mmu_map)(int pid, int page_no, int frame_no);
    int (*mmu_switch)(int pid);
    int (*mmu_translate)(int pid, int page_no);

    /* Devices interface */
    int (*disk_read)(int block_no, int nblocks, char* dst);
    int (*disk_write)(int block_no, int nblocks, char* src);

    int (*tty_intr)();
    int (*tty_read)(char* buf, int len);
    int (*tty_write)(wchar_t* buf, int len);

    int (*tty_printf)(const wchar_t *format, ...);
    int (*tty_info)(const wchar_t *format, ...);
    int (*tty_fatal)(const wchar_t *format, ...);
    int (*tty_success)(const wchar_t *format, ...);
    int (*tty_critical)(const wchar_t *format, ...);

    /* Some information about earth layer configuration */
    enum { QEMU, ARTY, ECE4750 } platform;
    enum { PAGE_TABLE, SOFT_TLB } translation;
};

struct grass {
    /* Shell environment variables */
    int workdir_ino;
    char workdir[128];

    /* Process control interface */
    int  (*proc_alloc)();
    void (*proc_free)(int pid);
    int  (*proc_entry)(int pid);
    void (*proc_set_ready)(int pid);

    /* System call interface */
    void (*sys_exit)(int status);
    int  (*sys_send)(int pid, char* msg, int size);
    int  (*sys_recv)(int* pid, char* buf, int size);
    void (*tmp_ecall)(int id);
};

extern struct earth *earth;
extern struct grass *grass;

/* Memory layout */
#define PAGE_SIZE          4096
#define FRAME_CACHE_END    0x00400000
#define FRAME_CACHE_START  0x00300000  /* 1MB    frame cache           */
                                       /*        earth interface       */
#define APPS_STACK_TOP     0x002FF000  /* 6KB    app stack             */
#define SYSCALL_ARG_OFFSET 0x00000400  /* 1KB    system call args      */
#define APPS_ARG           0x002FD000  /* 1KB    app main() argc, argv */
#define APPS_SIZE          0x00003000  
#define APPS_ENTRY         0x00200000  /* 12KB   app code+data         */
#define GRASS_STACK_TOP    0x001FF000  /* 8KB    earth/grass stack     */
                                       /*        grass interface       */
#define GRASS_SIZE         0x00003000  /* 12KB */
#define GRASS_ENTRY        0x00501000  /* 8KB    grass code+data       */
                                       /* 12KB   earth data            */
                                       /* earth code is in QSPI flash  */

#ifndef LIBC_STDIO
/* Only earth/dev_tty.c uses LIBC_STDIO and does not need these macros */
#define printf             earth->tty_printf
#define INFO               earth->tty_info
#define FATAL              earth->tty_fatal
#define SUCCESS            earth->tty_success
#define CRITICAL           earth->tty_critical
#endif

#undef memset
#define memset my_memset
#undef memcpy
#define memcpy my_memcpy

/* Memory-mapped I/O register access macros */
#define ACCESS(x) (*(__typeof__(*x) volatile *)(x))
#define REGW(base, offset) (ACCESS((unsigned int*)(base + offset)))
#define REGB(base, offset) (ACCESS((unsigned char*)(base + offset)))
