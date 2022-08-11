#include <kernel/vfs.h>

int fputchar(char c, file_descriptor_t file){
    vfs_write(file, &c, sizeof(c));
}