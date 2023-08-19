#include <types.h>
#include <stddef.h>

#include <kernel/list.h>

#define VFS_NEW_IMPL 0

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

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_DOT '.'

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
typedef int32_t (*read_block_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef int32_t (*write_block_type_t)(struct fs_node*, uint32_t block_number, uint32_t count, uint8_t* buffer);
typedef int (*get_file_size_type_t)(struct fs_node*);


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
  get_file_size_type_t get_file_size;
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



// path strings are parsed into this struct
// For example /a/b/c becomes a list with "a", "b" and "c"
typedef struct path {
    list_t* folders;
    char* path_string;
} path_t;

struct vfs_entry {
  fs_node_t* file;
  //char* device;
  //char* fs_type;
  //char* name;
};

typedef struct vfs_tree_node {
  //fs_node_t* fs_node;
  bool is_mount_point;
  fs_node_t* mount_point_fs_node;
  list_t* childrens;
  char* name;
  bool is_folder;
  //fs_node_t* root_current_fs;
} vfs_tree_node_t;



typedef struct {
  vfs_tree_node_t* root;
} vfs_tree_t;


struct vfs_tree_find_result {
    char* path_to_use;
    fs_node_t* fs_node;
};  



/*struct vfs_children {
  struct vfs_tree_node* tree_node;
  struct vfs_children_list* owner;
};*/

/*struct vfs_children_list {
  fs_node_t** childrens;
  size_t used;
  size_t allocated_size;
};*/


extern fs_node_t *fs_root; 
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint32_t flags);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name); 

fs_node_t* fs_get_root_node();

int vfs_write_fd(file_descriptor_t file, uint8_t* data, size_t size);
fs_node_t* vfs_open(const char* path, const char* mode);
void vfs_init();
int get_file_size_fs(fs_node_t *node);

void* vfs_mount(const char* path, fs_node_t* local_root);

#endif