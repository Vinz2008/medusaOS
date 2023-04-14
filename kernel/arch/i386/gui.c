#include <kernel/gui.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel/fb.h>
#include <kernel/graphics.h>
#include <kernel/mouse.h>
#include <kernel/list.h>
#include <kernel/kmalloc.h>
#include <kernel/x86.h>

#define WM_EVENT_QUEUE_SIZE 5

void wm_mouse_callback(mouse_t curr);

static uint32_t id_count = 0;
static fb_t fb;
static mouse_t mouse;
static window_t* focused;
static uint32_t background_color;
static window_t* background_window;
int row_putchar = 0;
int column_putchar = 8;
static window_t** window_list;
size_t window_list_used;
size_t window_list_size;

void init_window_list(){
    window_list = kmalloc(1 * sizeof(window_t*));
    window_list_used = 0;
    window_list_size = 1;
}

void add_to_window_list(window_t* window){
    if (window_list_used == window_list_size){
        window_list_size *= 2;
        window_list = krealloc(window_list, window_list_size * sizeof(window_t*));
    }
    window_list[window_list_used++] = window;
}

void empty_window_list(){
    kfree(window_list);
    window_list = NULL;
    window_list_size = window_list_used = 0;
}

rect_t rect_from_window(wm_window_t* win) {
    return (rect_t) {
        .top = win->pos.y,
        .left = win->pos.x,
        .bottom = win->pos.y + win->kfb.height - 1,
        .right = win->pos.x + win->kfb.width - 1
    };
}

void putchar_gui(char c){
    if (c == '\n'){
        row_putchar += 12;
        column_putchar = 8;
        return;
    }
    draw_char(focused->fb, c, column_putchar, row_putchar, 0xFFFFFF);
    column_putchar += 8;
    if (column_putchar >= focused->width - 16){
        column_putchar = 8;
        row_putchar += 12;
        if (row_putchar == focused->height){
            row_putchar = 0;
        }
    }
    render_window(focused);
}

void gui_keypress(char c){
    putchar_gui(c);
}

void move_window(window_t* window, int x, int y){
    if (window->x + x < 0 || window->x + x > fb.width){
        return;
    }
    if (window->y + y < 0 || window->y + y > fb.height){
        return;
    }
    fill_window_place(window);
    window->x += x;
    window->y += y;
    render_window(window);
}

void move_window_wm(window_t* window, enum direction dir){
    switch (dir){
        default:
        case LEFT:
            move_window(window, -100, 0);
            break;
        case RIGHT:
            move_window(window, 100, 0);
            break;
        case UP:
            move_window(window, 0, -100);
            break;
        case DOWN:
            move_window(window, 0, 100);
            break;
    }
}


void move_focused_window_wm(enum direction dir){
    move_window_wm(focused, dir);
}

uint32_t wm_open_window(fb_t* buff, uint32_t flags) {
    wm_window_t* win = (wm_window_t*) kmalloc(sizeof(wm_window_t));
    *win = (wm_window_t) {
        .ufb = *buff,
        .kfb = *buff,
        .id = ++id_count,
        .flags = flags | WM_NOT_DRAWN,
        .events = ringbuffer_new(WM_EVENT_QUEUE_SIZE * sizeof(wm_event_t))
    };
    win->kfb.address = (uintptr_t) kmalloc(buff->height*buff->pitch);
    //wm_assign_position(win);
    //wm_assign_z_orders();
    //wm_raise_window(win);
    return win->id;
}

void wm_draw_window(wm_window_t* win, rect_t rect) {
/*
    rect_t win_rect = rect_from_window(win);
    
    list_t* clip_windows = wm_get_windows_above(win);
    list_t clip_rects = LIST_HEAD_INIT(clip_rects);

    rect_add_clip_rect(&clip_rects, rect);

    // Convert covering windows to clipping rects
    while (!list_empty(clip_windows)) {
        wm_window_t* cw = list_first_entry(clip_windows, wm_window_t);
        list_del(list_first(clip_windows));
        rect_t clip = rect_from_window(cw);
        rect_subtract_clip_rect(&clip_rects, clip);
    }

    kfree(clip_windows);

    // Draw what's left
    rect_t* clip;
    list_for_each_entry(clip, &clip_rects) {
        if (rect_intersect(*clip, win_rect)) {
            wm_partial_draw_window(win, *clip);
        }
    }

    // Redraw the mouse
    //rect_t mouse_rect = wm_mouse_to_rect(mouse);
    //wm_draw_mouse(mouse_rect);

    rect_clear_clipped(&clip_rects);*/
}

void fill_screen(uint32_t col){
    for (int y = 0; y < fb.height; y++){
        for (int x = 0; x < fb.width; x++){
            draw_pixel(fb, x, y, col);
        }
    }
}

void clear_screen(){
    fill_screen(background_color);
}

void fill_window_place(window_t* win){
    for (int y = 0; y < win->height; y++){
        for (int x = 0; x < win->width; x++){
            uint32_t* offset_fb = pixel_offset(fb, x + win->x, y + win->y);
            offset_fb[0] = background_color;
        }
    }
}

void render_window(window_t* win){
    fb_t* wfb = &win->fb;
    uintptr_t fb_off = fb.address + fb.pitch + fb.bpp/8;
    uint32_t border_color2 = 0x00AA1100;
    //memcpy(fb.address, wfb->address, sizeof(wfb->height*wfb->pitch));
    log(LOG_SERIAL, false, "size window when rendering : %d\n", wfb->height*wfb->width*wfb->bpp/8);
    //memcpy((uint32_t*)fb.address, (uint32_t*)wfb->address, wfb->height*wfb->pitch);
    for (int y = 0; y < wfb->height; y++){
        for (int x = 0; x < wfb->width; x++){
            uint32_t* offset_window = pixel_offset(*wfb, x, y);
            uint32_t* offset_fb = pixel_offset(fb, x + win->x, y + win->y);
            offset_fb[0] = offset_window[0];
        }
    }
    draw_border(fb, win->x, win->y, win->width, win->height, border_color2);
}

void render_screen(){
    for (int i = 0; i < window_list_used; i++){
        render_window(window_list[i]);
    }
}

void wm_render_window(uint32_t win_id, rect_t* clip) {
	/*wm_window_t* win = wm_get_window(win_id);

	if (!win) {
		log(LOG_SERIAL, false, "[WM] Render called by invalid window, id %d\n", win_id);
		return;
	}
	// Copy the window's buffer in the kernel
	uint32_t win_size = win->ufb.height*win->ufb.pitch;
	memcpy((void*) win->kfb.address, (void*) win->ufb.address, sizeof(win->ufb.address));
	wm_refresh_partial(rect_from_window(win));*/
}

window_t* open_window(const char* title, int width, int height, uint32_t flags){
    window_t* win = (window_t*) kmalloc(sizeof(window_t));
    uint32_t bpp = 32;
    win->title = strdup(title);
    win->width = width;
    win->height = height;
    win->fb = (fb_t) {
        .address = (uintptr_t) zalloc(height*width*bpp/8),
        .pitch = width*bpp/8,
        .width = width,
        .height = height,
        .bpp = bpp,
    };

    //win->id = snow_wm_open_window(&win->fb, flags);
    win->flags = flags;
    focused = win;
    add_to_window_list(win);
    return win;
}


void draw_window(window_t* win){
    uint32_t bg_color = 0x353535;
    //uint32_t base_color = 0x222221;
    //uint32_t highlight = 0x030303;
    //uint32_t border_color = 0x000000;
    uint32_t text_color = 0xFFFFFF;
    uint32_t border_color2 = 0x121212;
    log(LOG_SERIAL, false, "Drawing window\n");
    draw_rectangle(win->fb, 10, 10, win->width, win->height, bg_color);
    draw_string(win->fb, win->title, 8, WM_TB_HEIGHT / 3, text_color);
    draw_border(win->fb, 10, 10, win->width, win->height, border_color2);
}


void init_gui(){
    log(LOG_SERIAL, false, "Starting GUI\n");
    fb = fb_get_info();
    background_window = open_window("background window", fb.width, fb.height, 0);
    mouse.x = fb.width/2;
    mouse.y = fb.height/2;
    //mouse_set_callback(wm_mouse_callback);
    //kbd_set_callback(wm_kbd_callback);
}
