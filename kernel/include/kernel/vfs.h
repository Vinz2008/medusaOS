#include <types.h>
#include <stddef.h>

#ifndef _VFS_HEADER_
#define _VFS_HEADER_
struct fs_node;

typedef int file_descriptor_t;

#define VFS_FD_STDIN 0
#define VFS_FD_STDOUT 1
#define VFS_FD_STDERR 2
#define VFS_FD_SERIAL 3

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

typedef enum FileType
{
    FT_File               = 1,
    FT_CharacterDevice    = 2,
    FT_BlockDevice        = 3,
    FT_Pipe               = 4,
    FT_SymbolicLink       = 5,
    FT_Directory          = 128,
    FT_MountPoint         = 256
} FileType;

#define READ_MODE 1
#define WRITE_MODE 2

typedef uint32_t (*read_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent* (*readdir_type_t)(struct fs_node*,uint32_t);
typedef struct fs_node* (*finddir_type_t)(struct fs_node*,char *name); 
typedef int32 (*read_block_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef int32 (*write_block_type_t)(struct fs_node*, uint32_t block_number, uint32_t count, uint8_t* buffer);

typedef struct fs_node
{
  char name[128];     // The filename.
  uint32_t mask;        // The permissions mask.
  uint32_t uid;         // The owning user.
  uint32_t gid;         // The owning group.
  uint32_t mode;
  uint32_t flags;       // Includes the node type (Directory, file etc).
  uint32_t inode;       // This is device-specific - provides a way for a filesystem to identify files.
  uint32_t length;      // Size of the file, in bytes.
  uint32_t impl;        // An implementation-defined number.
  read_type_t read;   // These typedefs are just function pointers. We'll define them later!
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir; // Returns the n'th child of a directory.
  finddir_type_t finddir; // Try to find a child in a directory by name.
  read_block_type_t read_block;
  write_block_type_t write_block;
  struct fs_node* mount_point; // Used by mountpoints and symlinks.
  struct fs_node* parent;
  void* private_node_data;
  uint32_t node_type;
} fs_node_t; 

struct dirent // One of these is returned by the readdir call, according to POSIX.
{
  char name[128]; // Filename.
  uint32_t ino;     // Inode number. Required by POSIX.
}; 

extern fs_node_t *fs_root; 
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name); 

fs_node_t* fs_get_root_node();

int vfs_write_fd(file_descriptor_t file, uint8_t* data, size_t size);
fs_node_t* vfs_open(const char* path, const char* mode);

#endif