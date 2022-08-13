#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <kernel/x86.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
uint16_t get_cursor_position(void);
void sys_key_handler(x86_iframe_t* frame);
void terminal_tick_init(uint8_t n);
void terminal_tick(char c);
void terminal_keypress_init(uint8_t n);
void terminal_keypress(uint8_t scan_code);

#endif
