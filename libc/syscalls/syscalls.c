#include <kernel/vfs.h>
#include <stddef.h>
#include <syscalls.h>

void write(int fd, void* buf, size_t size) { vfs_write_fd(fd, buf, size); }