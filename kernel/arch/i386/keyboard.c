#include <kernel/keyboard.h>
#include <types.h>
#include <stdio.h>
#include <ctype.h>
#include <kernel/irq_handlers.h>
#include <kernel/ps2.h>
#include <kernel/io.h>

#define KBD_DATA_PORT   0x60


uint8_t read_scan_code(void){
    return inb(KBD_DATA_PORT);
}


char keyboard_us[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
    0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char keyboard_us_shift[128] =
{
    0,  27, '!', '"', '£', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',   
  '\t', /* <-- Tab */
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',     
    0, /* <-- control key */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '@', '~',  0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char kbd_make_shift(char c) {
    if (c >= 'a' && c <= 'z') {
        return toupper(c);
    }
    switch (c) {
        case '`': return '~';
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        case '\\': return '|';
    }
    return c;
}

void kbd_ack(){
  while(!(inb(0x60)==0xfa));
}


void switch_led(int led){
  uint8_t led_status = 0;
  switch (led){ 
    case KBD_LED_SCROLL_LOCK:
      led_status ^= 1;
      break;
    case KBD_LED_NUMBER_LOCK:
      led_status ^= 2;
      break;
    case KBD_LED_CAPS_LOCK:
      led_status ^= 4;
      break;
  }
  log(LOG_SERIAL, false, "led status : %d\n", led_status);
  outb(0x60, 0xED);
  kbd_ack();
  outb(0x60, led_status);
}