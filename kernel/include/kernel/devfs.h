#include <kernel/vfs.h>

void devfs_initialize();

typedef struct Device {
  char name[16];
  FileType device_type;
  read_block_type_t read_block;
  write_block_type_t write_block;
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  void* private_data;
} Device;

struct dirent* devfs_readdir(struct fs_node* node, uint32_t index);
fs_node_t* devfs_finddir(struct fs_node* node, char* name);
fs_node_t* get_root_devfs();
fs_node_t* devfs_register_device(Device* device);