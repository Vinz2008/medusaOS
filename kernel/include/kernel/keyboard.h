#include <types.h>

#define KBD_SSC_CMD 0xF0
#define KBD_SSC_GET 0x00
#define KBD_SSC_2 0x02
#define KBD_SSC_3 0x03

typedef struct {
    uint8_t scancode[8];
    uint32_t current;
    bool alt;
    bool alt_gr;
    bool shift;
    bool super;
    bool control;
} kbd_context_t;

uint8_t read_scan_code(void);
void init_keyboard(uint32_t dev);

char kbd_keycode_to_char(uint32_t keycode, bool shift);
