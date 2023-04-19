#include <types.h>
#include <kernel/fb.h>
//#include <kernel/ringbuffer.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

//flags
#define NO_BORDER 0x01
#define NOT_VISIBLE 0x02
// then 0x04 and 0x08, etc

#define WM_NORMAL 0
#define WM_BACKGROUND 1
#define WM_FOREGROUND 2
#define WM_SKIP_INPUT 4
#define WM_TB_HEIGHT 30

#define WM_NOT_DRAWN  ((uint32_t) 1 << 31)

enum WM_EVENT {
    WM_EVENT_MOUSE_PRESS = 1,
    WM_EVENT_MOUSE_RELEASE,
    WM_EVENT_MOUSE_MOVE,
    WM_EVENT_MOUSE_ENTER,
    WM_EVENT_MOUSE_EXIT,
    WM_EVENT_KBD,
    WM_EVENT_GAINED_FOCUS,
    WM_EVENT_LOST_FOCUS,
};

enum direction {
    LEFT = 1,
    RIGHT,
    UP,
    DOWN
};

#define COLOR(_r, _g, _b)((uint8_t)( \
    (((_r) & 0x7) << 5) |       \
    (((_g) & 0x7) << 2) |       \
    (((_b) & 0x3) << 0)))


typedef struct {
    int32_t x, y;
} point_t;

typedef struct {
    char* title;
    uint32_t width;
    uint32_t height;
    fb_t fb;
    uint32_t id;
    uint32_t flags;
    int x;
    int y;
} window_t;

typedef struct _wm_window_t {
    fb_t ufb;
    fb_t kfb;
    point_t pos;
    uint32_t id;
    uint32_t flags;
    //ringbuffer_t* events; // some sort of list
    void* events;
} wm_window_t;

typedef struct {
    int32_t top, left, bottom, right;
} wm_rect_t;

typedef struct {
    wm_rect_t position;
    bool left_button;
    bool right_button;
} wm_click_event_t;

typedef struct {
    uint32_t keycode;
    bool pressed;
    char repr;
    // TODO include modifiers
} wm_kbd_event_t;

/* This structure is how a window can get its user input.
 * `type` is one of the `WM_EVENT_*` flags, and describe valid fields:
 *  - `WM_EVENT_{CLICK,MOUSE_MOVE}` -> `mouse` is valid,
 *  - `WM_EVENT_KBD` -> `kbd` is valid.
 */
typedef struct {
    uint32_t type;
    wm_click_event_t mouse;
    wm_kbd_event_t kbd;
} wm_event_t;

typedef wm_rect_t rect_t;

void init_gui();
window_t* open_window(const char* title, int width, int height, uint32_t flags);
void draw_window(window_t* win);
void move_focused_window_wm(enum direction dir);