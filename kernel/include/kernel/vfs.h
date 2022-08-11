#include <types.h>
#include <stddef.h>

typedef int file_descriptor_t;

#define VFS_FD_STDIN 0
#define VFS_FD_STDOUT 1
#define VFS_FD_STDERR 2
#define VFS_FD_SERIAL 3

int vfs_write(file_descriptor_t file, uint8_t* data, size_t size);