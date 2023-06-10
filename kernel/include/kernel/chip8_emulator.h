#include <stdbool.h>
#include <stdint.h>
#include <kernel/stack.h>

#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32

void setup_chip8_emulator(const char* filename);
bool is_chip8_emulator_mode();

typedef struct Chip8_emulator {
    uint8_t ram[4096];
    uint8_t vregisters[16];
    uint16_t i;
    int delay_timer;
    uint16_t pc;
    stack_t* stack;
    uint8_t speed;
    bool paused;
    bool display[CHIP8_SCREEN_WIDTH*CHIP8_SCREEN_HEIGHT];
} chip8_emulator_t;