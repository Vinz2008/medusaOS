#include <kernel/vfs.h>

typedef struct _pipe_device {
  uint8_t* buffer;
  size_t write_ptr;
  size_t read_ptr;
  size_t size;
  size_t refcount;
  uint8_t volatile lock;
  void* wait_queue;
} pipe_device_t;

fs_node_t* make_pipe(size_t size);