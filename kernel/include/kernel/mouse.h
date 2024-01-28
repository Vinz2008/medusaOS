#include <types.h>

#define MOUSE_STATUS 0x64
#define MOUSE_PORT 0x60
#define MOUSE_BBIT 0x01
#define MOUSE_F_BIT 0x20
#define MOUSE_WRITE 0xD4

// Mouse-specific commands
#define MOUSE_SET_SAMPLE 0xF3
#define MOUSE_SET_RESOLUTION 0xE8
#define MOUSE_ENABLE_SCALING 0xE7
#define MOUSE_DISABLE_SCALING 0xE7

// Bits of the first mouse data packet
#define MOUSE_Y_OVERFLOW (1 << 7)
#define MOUSE_X_OVERFLOW (1 << 6)
#define MOUSE_Y_NEG (1 << 5)
#define MOUSE_X_NEG (1 << 4)
#define MOUSE_ALWAYS_SET (1 << 3)
#define MOUSE_MIDDLE (1 << 2)
#define MOUSE_RIGHT (1 << 1)
#define MOUSE_LEFT (1 << 0)

// Bits of the fourth mouse data packet
#define MOUSE_UNUSED_A (1 << 7)
#define MOUSE_UNUSED_B (1 << 6)

#define MOUSE_SET_DEFAULTS 0xF6
#define MOUSE_DATA_ON 0xF4

typedef struct {
  float x, y;
  bool left_pressed;
  bool right_pressed;
  bool middle_pressed;
} mouse_t;

void mouse_install();