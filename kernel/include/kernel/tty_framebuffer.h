#include <types.h>

#define WHITE 0xFFFFFF
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLACK 0x000000

void terminal_framebuffer_initialize();
void terminal_framebuffer_putc(char c);
void terminal_framebuffer_keypress(char c);
void terminal_framebuffer_setcolor(uint32_t col);
void terminal_framebuffer_reset_color();
void empty_line_cli_framebuffer();
void launch_command_framebuffer();
char* get_line_cli();
void terminal_framebuffer_putc_back(char c);
void terminal_framebuffer_delete_character();