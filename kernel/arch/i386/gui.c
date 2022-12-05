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
//static wm_window_t* focused;


rect_t rect_from_window(wm_window_t* win) {
    return (rect_t) {
        .top = win->pos.y,
        .left = win->pos.x,
        .bottom = win->pos.y + win->kfb.height - 1,
        .right = win->pos.x + win->kfb.width - 1
    };
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


void render_window(window_t* win){
    fb_t* wfb = &win->fb;
    uintptr_t fb_off = fb.address + fb.pitch + fb.bpp/8;
    //memcpy(fb.address, wfb->address, sizeof(wfb->height*wfb->pitch));
    log(LOG_SERIAL, false, "size window when rendering : %d\n", wfb->height*wfb->width*wfb->bpp/8);
    memcpy((uint32_t*)fb.address, (uint32_t*)wfb->address, wfb->height*wfb->pitch);
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
    mouse.x = fb.width/2;
    mouse.y = fb.height/2;
    //mouse_set_callback(wm_mouse_callback);
    //kbd_set_callback(wm_kbd_callback);
}
