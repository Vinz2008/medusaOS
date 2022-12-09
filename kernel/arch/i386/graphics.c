#include <kernel/graphics.h>
#include <types.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <kernel/fb.h>
#include <kernel/config.h>
#if GUI_MODE
#include <kernel/font.h>
#endif


typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t height;
} font_header_t;


uint32_t* pixel_offset(fb_t fb, uint32_t x, uint32_t y){
    return (uint32_t*) (fb.address + y * fb.pitch + x * fb.bpp/8);
}

void draw_pixel(fb_t fb, int x, int y, uint32_t col){
    uint32_t* offset = pixel_offset(fb, x, y);
    *offset = col;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

void draw_line_low(fb_t fb, int x0, int y0, int x1, int y1, uint32_t col){
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0){
        yi = -1;
        dy = -dy;
    }
    int D = 2 * dy - dx;
    int y = y0;
    for (int x = x0; x < x1; x++){
        draw_pixel(fb ,x, y, col);
        if (D > 0){
            y += yi;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}

void draw_line_high(fb_t fb, int x0, int y0, int x1, int y1, uint32_t col){
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0){
        xi = -1;
        dx = -dx;
    }
    int D = 2 * dx - dy;
    int x = x0;
    for (int y = y0; y < y1; y++){
        draw_pixel(fb, x, y, col);
        if (D > 0){
            x = x + xi;
            D = D + 2 * (dx - dy);
        } else {
            D = D + 2 * dx;
        }
    }
}

void draw_line_horizontal(fb_t fb, int x0, int x1, int y, uint32_t col){
    if (x0 > x1){
        int x = x0;
        x0 = x1;
        x1 = x;
    }
    uint32_t* offset = pixel_offset(fb, x0, y);
    for (int i = 0; i <= x1 - x0; i++){
        offset[i] = col;
    }
}

void draw_line_vertical(fb_t fb, int x, int y0, int y1, uint32_t col){
    if (y0 > y1){
        int y = y0;
        y0 = y1;
        y1 = y;
    }
    uint32_t* offset = pixel_offset(fb, x, y0);
    for (int i = 0; i <= y1 - y0; i++){
        *offset = col;
        offset = (uint32_t*)((uintptr_t) offset + fb.pitch);
    }
}

void draw_line(fb_t fb, int x0, int y0, int x1, int y1, uint32_t col){
    if (y0 == y1){
        draw_line_horizontal(fb, x0, x1, y0, col);
        return;
    }
    if (x0 == x1){
        draw_line_vertical(fb, x0, y0, y1, col);
    }
    if (abs(y1 - y0) < abs(x1 - x0)){
        if (x0 > x1){
            draw_line_low(fb, x1, y1, x0, y0, col);
        } else {
            draw_line_low(fb, x0, y0, x1, y1, col);
        }
    } else {
        if (y0 > y1){
            draw_line_high(fb, x1, y1, x0, y0, col);
        } else {
            draw_line_high(fb, x0, y0, x1, y1, col);
        }
    }
}


void draw_char(fb_t fb, char c, int x, int y, uint32_t col){
    log(LOG_SERIAL, false, "draw character: %c x : %d y : %d col : %d\n", c, x, y, col);
#if GUI_MODE
    uint8_t* offset = font_psf + sizeof(font_header_t) + c*16;
    for (int i = 0; i < 16; i ++){
        for (int j = 0; j < 8; j++){
            if (offset[i] & (1 << j)){
                draw_pixel(fb, x + 8 - j, y + i, col);
            }
        }
    }
#endif
}


void draw_string(fb_t fb, const char* str, int x, int y, uint32_t col){
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        draw_char(fb, str[i], x + 8*i, y, col);
    }
}

void draw_rectangle(fb_t fb, int x, int y, int w, int h, uint32_t col){
    uint32_t* offset = pixel_offset(fb, x, y);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            offset[j] = col;
        }
        offset = (uint32_t*) ((uintptr_t) offset + fb.pitch);
    }
}

void draw_border(fb_t fb, int x, int y, int w, int h, uint32_t col){
    draw_line_horizontal(fb, x, x + w - 1, y, col);
    draw_line_horizontal(fb, x, x + w - 1, y + h - 1, col);
    draw_line_vertical(fb, x, y, y + h - 1, col);
    draw_line_vertical(fb, x + w - 1, y, y + h - 1, col);
}
