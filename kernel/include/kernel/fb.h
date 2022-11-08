#include <multiboot2.h>

typedef struct {
    uintptr_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
} fb_t;

void init_fb(struct multiboot_tag_framebuffer *tagfb);
fb_t fb_get_info();