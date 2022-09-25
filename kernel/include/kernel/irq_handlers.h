#include <kernel/x86.h>

// keycodes : https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1

enum Key {
    ESC_KEY = 0x01,
    ENTER_KEY = 0x1C,
    DELETE_KEY = 0x53,
    TAB_KEY = 0x0F,
    CURSOR_LEFT_KEY = 0x4B,
    CURSOR_UP_KEY = 0x75
};

#define IRQ_PIT 0
#define IRQ_PS2 1

void handle_platform_irq(x86_iframe_t* frame);
void irq_register_handler(int irq, void (*handler)(x86_iframe_t*));