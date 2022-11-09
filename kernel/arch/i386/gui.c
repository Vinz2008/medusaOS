#include <kernel/gui.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/fb.h>
#include <kernel/graphics.h>
#include <kernel/mouse.h>
#include <kernel/kmalloc.h>
#include <kernel/x86.h>


void wm_mouse_callback(mouse_t curr);

static fb_t fb;
static mouse_t mouse;

window_t* open_window(const char* title, int width, int height, uint32_t flags){
    window_t* win = (window_t*) kmalloc(sizeof(window_t));
    uint32_t bpp = 32;
    win->title = strdup(title);
    win->width = width;
    win->height = height;
    win->fb = (fb_t) {
        .address = (uintptr_t) zalloc(width*height*bpp/8),
        .pitch = width*bpp/8,
        .width = width,
        .height = height,
        .bpp = bpp,
    };

    //win->id = snow_wm_open_window(&win->fb, flags);
    win->flags = flags;

    return win;
}


void draw_window(window_t* win){
    uint32_t bg_color = 0x353535;
    uint32_t base_color = 0x222221;
    uint32_t highlight = 0x030303;
    uint32_t border_color = 0x000000;
    uint32_t text_color = 0xFFFFFF;
    uint32_t border_color2 = 0x121212;
    draw_rectangle(win->fb, 0, 0, win->width, win->height, bg_color);
    draw_string(win->fb, win->title, 8, WM_TB_HEIGHT / 3, text_color);
    draw_border(win->fb, 0, 0, win->width, win->height, border_color2);
}

void init_gui(){
    log(LOG_SERIAL, false, "Starting GUI\n");
    fb = fb_get_info();
    mouse.x = fb.width/2;
    mouse.y = fb.height/2;
    //mouse_set_callback(wm_mouse_callback);
    //kbd_set_callback(wm_kbd_callback);



}
