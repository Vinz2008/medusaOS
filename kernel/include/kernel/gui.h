#include <types.h>
#include <kernel/fb.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#define WM_TB_HEIGHT 30

#define COLOR(_r, _g, _b)((uint8_t)( \
    (((_r) & 0x7) << 5) |       \
    (((_g) & 0x7) << 2) |       \
    (((_b) & 0x3) << 0)))

typedef struct {
    char* title;
    uint32_t width;
    uint32_t height;
    fb_t fb;
    uint32_t id;
    uint32_t flags;
} window_t;

void init_gui();
window_t* open_window(const char* title, int width, int height, uint32_t flags);
void draw_window(window_t* win);