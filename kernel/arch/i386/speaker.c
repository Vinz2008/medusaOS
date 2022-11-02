#include <kernel/speaker.h>
#include <kernel/io.h>
#include <kernel/x86.h>
#include <types.h>
 
static void play_sound(uint32_t nFrequence) {
    uint32_t div;
    uint8_t tmp;
    //Set the PIT to the desired frequency
 	div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (div) );
 	outb(0x42, (uint8_t) (div >> 8));
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
    }
}
 
 //make it shutup
static void nosound() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 	outb(0x61, tmp);
}
 
 //Make a beep
 void beep() {
 	play_sound(1000);
    x86_halt();
 	//timer_wait(10);
 	nosound();
    //set_PIT_2(old_frequency);
 }