#include <stdio.h>
#include <kernel/fb.h>
#include <kernel/graphics.h>
#include <kernel/config.h>

#if GUI_MODE
#else
#include <kernel/font.h>
#endif

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t height;
} font_header_t;

static fb_t fb;
int row = 0;
int column = 0;
uint32_t color = 0xFFFFFF;

void terminal_framebuffer_initialize(){
    log(LOG_SERIAL, false, "Starting terminal framebuffer\n");
    fb = fb_get_info();
}

void terminal_framebuffer_putc(char c){
#if GUI_MODE
#else
    if (c == '\n'){
        row += 12;
        column = 0;
        return;
    }
    uint8_t* offset = font_psf + sizeof(font_header_t) + c*16;
    for (int i = 0; i < 16; i ++){
        for (int j = 0; j < 8; j++){
            if (offset[i] & (1 << j)){
                //draw_pixel(fb, x + 8 - j, y + i, col);
                draw_pixel(fb, column + 8 - j, row + i, color);
            }
        }
    }
    column += 8;
#endif
}
