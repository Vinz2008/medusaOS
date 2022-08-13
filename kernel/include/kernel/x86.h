#include <types.h>

#ifndef __x86_H__
#define __x86_H__
typedef struct x86_32_iframe {
    uint32_t di, si, bp, sp, bx, dx, cx, ax; 
    uint32_t ds, es, fs, gs;                        
    uint32_t vector;                                 
    uint32_t err_code;                                
    uint32_t ip, cs, flags;                            
    uint32_t user_sp, user_ss;                          
} x86_iframe_t ;

static uint64_t ticks = 0;


static inline void x86_enable_int(void){
    __asm__ volatile("sti");
}

static inline void x86_disable_int(void){
    __asm__ volatile("cli");
}

static inline void x86_halt(void){
    __asm__ volatile(
		    "cli;"
		    "hlt");
}
#endif