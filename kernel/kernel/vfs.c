#include <kernel/vfs.h>
#include <stdio.h>
#include <kernel/serial.h>

fs_node_t *fs_root = 0;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
  // Has the node got a read callback?
  if (node->read != 0)
    return node->read(node, offset, size, buffer);
  else
    return 0;
} 

void open_fs(fs_node_t *node, uint8_t read, uint8_t write){
  // Has the node got a read callback?
  if (node->read != 0)
    return node->open(node);
  else
    return 0;
} 

void close_fs(fs_node_t *node){
  // Has the node got a read callback?
  if (node->read != 0)
    return node->close(node);
  else
    return 0;
} 

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
  // Has the node got a read callback?
  if (node->read != 0)
    return node->write(node, offset, size, buffer);
  else
    return 0;
} 



// write a character
int vfs_write_fd(file_descriptor_t file, uint8_t* data, size_t size){
    if (file == VFS_FD_STDOUT){
        putchar(*data);
    } else if (file == VFS_FD_SERIAL){
        write_serial_char(*data);
    } else if (file == VFS_FD_STDERR){
        alert("%c", *data);
    }

}