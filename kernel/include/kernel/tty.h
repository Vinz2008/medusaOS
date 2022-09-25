#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <kernel/x86.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_setcolors(uint8_t fg, uint8_t bg);
void terminal_reset_color();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
void move_cursor_left();
void move_cursor_right();
void move_cursor_next_line();
void move_cursor_last_line();
uint16_t get_cursor_position(void);
void sys_key_handler(x86_iframe_t* frame);
void terminal_tick_init(uint8_t n);
void terminal_tick(char c);
void terminal_keypress_init(uint8_t n);
void terminal_keypress(uint8_t scan_code);
void terminal_print_last_command();
void empty_line_cli();
void remove_character();
void launch_command();
void terminal_disable_keypress();
void terminal_disable_tick();

#endif
