/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple tty device driver; getc and printf functions
 * have been implemented by the freedom-metal library for Arty
 */


#include "earth.h"

#define ENTER  0x0d
#define CTRL_C 0x03

int metal_tty_getc(int *c);
int tty_read(char* buf, int len) {
    for (int c, i = 0; i < len - 1; i++) {
        for (c = -1; c == -1; metal_tty_getc(&c));
        buf[i] = (char)c;

        switch (c) {
        case ENTER:
            printf("\r\n");
            buf[i] = 0;
            return 0;
        case CTRL_C:
            printf("\r\n");
            buf[0] = 0;
            return 0;
        }

        printf("%c", c);
        fflush(stdout);
    }

    buf[len - 1] = 0;
    return 0;
}

int tty_write(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int r = vprintf(format, args);
    va_end(args);

    return r < 0 ? r : fflush(stdout);
}

int tty_init() {
    struct metal_uart* uart;
    uart = metal_uart_get_device(0);
    metal_uart_init(uart, 115200);
    for (int i = 0; i < 2000000; i++);
    
    if (!uart) {
        ERROR("Unable to get uart handle");
        return -1;
    }

    /* wait for the tty device to be ready */
    for (int c = 0; c != -1; metal_uart_getc(uart, &c));
    return 0;
}
