#include <kernel/vfs.h>
#include <stdio.h>
#include <kernel/serial.h>


// write a character
int vfs_write(file_descriptor_t file, uint8_t* data, size_t size){
    if (file == VFS_FD_STDOUT){
        putchar(*data);
    }
    if (file == VFS_FD_SERIAL){
        write_serial_char(*data);
    }

}