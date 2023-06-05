#include <types.h>
#include <kernel/fb.h>

void draw_line_horizontal(fb_t fb, int x0, int x1, int y, uint32_t col);
void draw_line_vertical(fb_t fb, int x, int y0, int y1, uint32_t col);
void draw_line(fb_t fb, int x0, int y0, int x1, int y1, uint32_t col);
void draw_char(fb_t fb, char c, int x, int y, uint32_t col);
void draw_string(fb_t fb, const char* str, int x, int y, uint32_t col);
void draw_string_size(fb_t fb, int size_mult, const char* str, int x, int y, uint32_t col);
void draw_rectangle(fb_t fb, int x, int y, int w, int h, uint32_t col);
void draw_border(fb_t fb, int x, int y, int w, int h, uint32_t col);
void draw_pixel(fb_t fb, int x, int y, uint32_t col);
void draw_char(fb_t fb, char c, int x, int y, uint32_t col);
void fill_screen(fb_t fb, uint32_t col);


#define RGB(r, g, b) (uint32_t)(r << 16 | g << 8 | b )

#define COLOR_WHITE 0xFFFFFF
#define COLOR_RED 0xFF0000
#define COLOR_GREEN 0x90EE90
#define COLOR_BLUE 0x0000FF