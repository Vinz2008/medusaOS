#include <kernel/x86.h>

// keycodes : https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1

enum Key {
    ESC_KEY = 0x01,
    ENTER_KEY = 0x1C,
    DELETE_KEY = 0x53,
    TAB_KEY = 0x0F,
    CURSOR_LEFT_KEY = 0x4B,
    CURSOR_UP_KEY = 0x75,
    BACKSPACE_KEY = 0x0E
};

#define IRQ_PIT 0
#define IRQ_PS2 1

void handle_platform_irq(registers_t* frame);
void irq_register_handler(int irq, void (*handler)(registers_t*));
void sys_key_handler(registers_t* frame);
void sys_mouse_handler(registers_t* frame);
void sys_tick_handler(registers_t* frame);