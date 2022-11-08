#include <types.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

#define COLOR(_r, _g, _b)((uint8_t)( \
    (((_r) & 0x7) << 5) |       \
    (((_g) & 0x7) << 2) |       \
    (((_b) & 0x3) << 0)))

void init_gui();
void draw_line_horizontal(int x0, int x1, int y, uint32_t col);
void draw_line_vertical(int x, int y0, int y1, uint32_t col);
void draw_line(int x0, int y0, int x1, int y1, uint32_t col);
void draw_char(char c, int x, int y, uint32_t col);
void draw_string(const char* str, int x, int y, uint32_t col);