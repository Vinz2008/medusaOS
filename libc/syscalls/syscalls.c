#include <syscalls.h>
#include <stddef.h>
#include <kernel/vfs.h>


void write(int fd, void *buf, size_t size){
    vfs_write(fd, buf, size);
}