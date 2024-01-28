#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct ringbuffer_t {
  size_t size;
  size_t r_base;
  size_t data_size;
  uint8_t* data;
} ringbuffer_t;

ringbuffer_t* ringbuffer_init(ringbuffer_t* ref, uint8_t* buf, size_t size);
ringbuffer_t* ringbuffer_new(size_t size);
size_t ringbuffer_available(ringbuffer_t* ref);
void ringbuffer_free(ringbuffer_t* ref);
size_t ringbuffer_write(ringbuffer_t* ref, size_t n, uint8_t* buffer);
size_t ringbuffer_read(ringbuffer_t* ref, size_t n, uint8_t* buffer);