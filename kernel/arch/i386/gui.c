#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <kernel/fb.h>
#include <kernel/gui.h>
#include <kernel/font.h>
#include <kernel/x86.h>

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t height;
} font_header_t;


static fb_t fb;

uint32_t* pixel_offset(uint32_t x, uint32_t y){
    return (uint32_t*) (fb.address + y * fb.pitch + x * fb.bpp/8);
}

void init_gui(){
    log(LOG_SERIAL, false, "Starting GUI\n");
    fb = fb_get_info();
    uint32_t* offset = (uint32_t*) (fb.address + fb.height*fb.pitch + fb.width*fb.bpp/8);


}

void draw_pixel(int x, int y, uint32_t col){
    uint32_t* offset = pixel_offset(x, y);
    *offset = col;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

void draw_line_low(int x0, int y0, int x1, int y1, uint32_t col){
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
        draw_pixel(x, y, col);
        if (D > 0){
            y += yi;
            D -= 2*dx;
        }
        D += 2*dy;
    }
}

void draw_line_high(int x0, int y0, int x1, int y1, uint32_t col){
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
        draw_pixel(x, y, col);
        if (D > 0){
            x = x + xi;
            D = D + 2 * (dx - dy);
        } else {
            D = D + 2 * dx;
        }
    }
}

void draw_line_horizontal(int x0, int x1, int y, uint32_t col){
    if (x0 > x1){
        int x = x0;
        x0 = x1;
        x1 = x;
    }
    uint32_t* offset = pixel_offset(x0, y);
    for (int i = 0; i <= x1 - x0; i++){
        offset[i] = col;
    }
}

void draw_line_vertical(int x, int y0, int y1, uint32_t col){
    if (y0 > y1){
        int y = y0;
        y0 = y1;
        y1 = y;
    }
    uint32_t* offset = pixel_offset(x, y0);
    for (int i = 0; i <= y1 - y0; i++){
        *offset = col;
        offset = (uint32_t*)((uintptr_t) offset + fb.pitch);
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t col){
    if (y0 == y1){
        draw_line_horizontal(x0, x1, y0, col);
        return;
    }
    if (x0 == x1){
        draw_line_vertical(x0, y0, y1, col);
    }
    if (abs(y1 - y0) < abs(x1 - x0)){
        if (x0 > x1){
            draw_line_low(x1, y1, x0, y0, col);
        } else {
            draw_line_low(x0, y0, x1, y1, col);
        }
    } else {
        if (y0 > y1){
            draw_line_high(x1, y1, x0, y0, col);
        } else {
            draw_line_high(x0, y0, x1, y1, col);
        }
    }
}

void draw_char(char c, int x, int y, uint32_t col){
    uint8_t* offset = font_psf + sizeof(font_header_t) + c*16;
    for (int i = 0; i < 16; i ++){
        for (int j = 0; j < 8; j++){
            if (offset[i] & (1 << j)){
                draw_pixel(x + 8 - j, y + i, col);
            }
        }
    }
}

void draw_string(const char* str, int x, int y, uint32_t col){
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        draw_char(str[i], x + 8*i, y, col);
    }
}