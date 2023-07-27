/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: a simple tty device driver
 * uart_getc() and uart_putc() are implemented in bus_uart.c
 * printf-related functions are linked from the compiler's C library
 */

#define LIBC_STDIO
#include "egos.h"
#include <stdarg.h>

void ece4750_wprint_int( int i )
{
    asm( "csrw 0x7c0, %0" :: "r"(0x00030000) );
    asm( "csrw 0x7c0, %0" :: "r"(i) );
}

void ece4750_wprint_char( wchar_t c )
{
    asm( "csrw 0x7c0, %0" :: "r"(0x00030001) );
    asm( "csrw 0x7c0, %0" :: "r"(c) );
}

void ece4750_wprint_str( const wchar_t* p )
{
    asm( "csrw 0x7c0, %0" :: "r"(0x00030002) );
    while ( *p != 0 ) {
        asm( "csrw 0x7c0, %0" :: "r"(*p) );
        p++;
    }
    asm( "csrw 0x7c0, %0" :: "r"(*p) );
}

int uart_getc(int* c);
void uart_putc(int c);
void uart_init(long baud_rate);

int tty_write(wchar_t* buf, int len) {
    for (int i = 0; i < len; i++)
        ece4750_wprint_char(buf[i]);
}

void my_vprintf( const wchar_t* fmt,  va_list args ) {
    int flag = 0;
    while (*fmt != '\0') {
        if (*fmt == '%' ) {
            flag = 1;
        }
        else if ( flag && (*fmt == 'd') ) {
            ece4750_wprint_int( va_arg(args, int) );
            flag = 0;
        }
        else if ( flag && (*fmt == 'c') ) {
            // note automatic conversion to integral type
            ece4750_wprint_char( (wchar_t) (va_arg(args, int)) );
            flag = 0;
        }
        else if ( flag && (*fmt == 's') ) {
            ece4750_wprint_str( va_arg(args, wchar_t*) );
            flag = 0;
        }
        else {
            ece4750_wprint_char( *fmt );
        }
        ++fmt;
    }
    asm( "csrw 0x7c0, %0" :: "r"(0x00030003) );  /* Flush */
}

void my_printf( const wchar_t* fmt, ... )
{
  va_list args;
  va_start(args, fmt);
  my_vprintf(fmt, args);
  va_end(args);
}

#define LOG(x, y)  ece4750_wprint_str(x); \
                   va_list args; \
                   va_start(args, format); \
                   my_vprintf(format, args); \
                   va_end(args); \
                   ece4750_wprint_str(y); \

int tty_printf(const wchar_t *format, ...)
{
    LOG(L"", L"")
}

int tty_info(const wchar_t *format, ...) { LOG(L"[INFO] ", L"\r\n") }

int tty_fatal(const wchar_t *format, ...)
{
    LOG(L"\x1B[1;31m[FATAL] ", L"\x1B[1;0m\r\n") /* red color */
    asm( "csrw 0x7c0, %0" :: "r"(0x10000) );
}

int tty_success(const wchar_t *format, ...)
{
    LOG(L"\x1B[1;32m[SUCCESS] ", L"\x1B[1;0m\r\n") /* green color */
}

int tty_critical(const wchar_t *format, ...)
{
    LOG(L"\x1B[1;33m[CRITICAL] ", L"\x1B[1;0m\r\n") /* yellow color */
}

static int c, is_reading;
int tty_intr() { return (is_reading)? 0 : (uart_getc(&c) == 3); }

int tty_read(wchar_t* buf, int len) {
    is_reading = 1;
    for (int i = 0; i < len - 1; i++) {
        //for (c = -1; c == -1; uart_getc(&c));
        c = -1;
        while (c == -1 || c == 0) {
          asm( "csrr %0, 0xfc0" : "=r"(c) );
        }
        asm( "csrw 0x7c0, %0" :: "r"(0x00040000) );

        buf[i] = c;
        if (c == 10) {  /* Enter */
            buf[i] = 0;
            return i;
        }
    }

    buf[len - 1] = is_reading = 0;    
    return len - 1;
}

void tty_init() {
    earth->tty_intr = tty_intr;
    earth->tty_read = tty_read;
    earth->tty_write = tty_write;
    
    earth->tty_printf = tty_printf;
    earth->tty_info = tty_info;
    earth->tty_fatal = tty_fatal;
    earth->tty_success = tty_success;
    earth->tty_critical = tty_critical;
}