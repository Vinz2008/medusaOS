#include <kernel/vfs.h>

int fputchar(char c, file_descriptor_t file) {
  vfs_write_fd(file, (uint8_t*)&c, sizeof(c));
}