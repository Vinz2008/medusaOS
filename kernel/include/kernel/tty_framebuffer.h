#include <types.h>

#define WHITE 0xFFFFFF
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLACK 0x000000

void terminal_framebuffer_initialize();
void terminal_framebuffer_putc(char c);
void terminal_framebuffer_keypress(uint8_t scan_code);