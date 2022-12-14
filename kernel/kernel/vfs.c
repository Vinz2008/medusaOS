#include <kernel/vfs.h>
#include <stdio.h>
#include <string.h>
#include <kernel/kmalloc.h>
#include <kernel/serial.h>

fs_node_t *fs_root = NULL;
vfs_tree_t* fs_tree = NULL;

vfs_tree_t* vfs_tree_init(){
  vfs_tree_t* tree = kmalloc(sizeof(vfs_tree_t));
  tree->nodes = 0;
  tree->root = NULL;
  return tree;
}

vfs_tree_node_t* vfs_tree_node_create(struct vfs_entry* vfs_node){
  vfs_tree_node_t* temp = kmalloc(sizeof(vfs_tree_node_t));
  temp->vfs_entry_file = vfs_node;
  temp->children_list = kmalloc(sizeof(struct vfs_children_list));
  temp->children_list->childrens = kmalloc(sizeof(struct vfs_children) * 10);
  temp->children_list->used = 0;
  temp->parent = NULL;
  return temp;
}

void vfs_tree_set_root(vfs_tree_t* fs_tree, struct vfs_entry* vfs_node){
  vfs_tree_node_t* root = vfs_tree_node_create(vfs_node);
  fs_tree->root = root;
  fs_tree->nodes = 1;
}


void vfs_init(){
  fs_tree = vfs_tree_init;
  struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));
  root->name = strdup("[root]");
  root->file = NULL;
  root->fs_type = NULL;
  root->device = NULL;
  vfs_tree_set_root(fs_tree, root);
}

void* vfs_mount(const char* path, fs_node_t* local_root){
  vfs_tree_node_t* ret_val = NULL;
  char* p = strdup(path);
  vfs_tree_node_t* root_node = fs_tree->root;
  if (*path == '/'){
    struct vfs_entry* root = (struct vfs_entry*)root_node->vfs_entry_file;
    if (root->file){
      log(LOG_SERIAL, false, "Path %s already mounted\n", path);
    }
    root->file = local_root;

  } else {
    vfs_tree_node_t* node = root_node;
    while (1){
      int found = 0;
      for (int i = 0; i < node->children_list->used; i++){
        vfs_tree_node_t* tchild = (vfs_tree_node_t*)node->children_list->childrens[i].vfs_entry_file;
      }
    }
  }
  return ret_val;
}

int get_file_size_fs(fs_node_t *node){
  if (node == NULL){
	log(LOG_SERIAL, false, "NULL in get_file_size_fs\n");
    return -1;
  }
  if (node->get_file_size != 0){
    return node->get_file_size(node);
  } else {
    return -1;
  }
}

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
  // Has the node got a read callback?
  if (node == NULL){
	log(LOG_SERIAL, false, "NULL in read_fs\n");
    return;
  }
  if (node->read != 0)
    return node->read(node, offset, size, buffer);
  else
    return 0;
} 

void open_fs(fs_node_t *node, uint8_t read, uint8_t write){
  // Has the node got a read callback?
  if (node->open != 0)
    return node->open(node);
  else {
    if (read == 1){
      node->mode =  READ_MODE;
    } else if (write == 1){
      node->mode = WRITE_MODE;
    }
    //return;
  }
} 

void close_fs(fs_node_t *node){
  // Has the node got a read callback?
  if (node == NULL){
	return;
  }
  if (node->close != 0)
    return node->close(node);
  else
	node->mode = 0;
    //return;
} 

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
  // Has the node got a read callback?
  if (node->write != 0)
    return node->write(node, offset, size, buffer);
  else
    return 0;
} 

struct dirent *readdir_fs(fs_node_t *node, uint32_t index){
	// Is the node a directory, and does it have a callback?
	/*if ( (node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0 )
		return node->readdir(node, index);
	else
		return 0;*/
  if (node->readdir == 0){
    return 0;
  }
  struct dirent* dir = node->readdir(node, index);
  if (strcmp("dev", dir->name) == 0){
    log(LOG_SERIAL, false, "DEV FOUND\n");
    return dir;
  } else {
    return dir;
  }
}

fs_node_t *finddir_fs(fs_node_t *node, char *name){
	// Is the node a directory, and does it have a callback?
	if ( (node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0 )
		return node->finddir(node, name);
	else
		return 0;
}



fs_node_t* fs_get_root_node(){
  return fs_root;
}

uint32_t vfs_get_depth(const char *path){
  uint32_t depth = 0;

   for(size_t i = 0; path[i] != '\0'; ++i)
      if(path[i] == '/')
         ++depth;

   return depth;
}

// write a character
int vfs_write_fd(file_descriptor_t file, uint8_t* data, size_t size){
    (void)size;
    if (file == VFS_FD_STDOUT){
        putchar(*data);
    } else if (file == VFS_FD_SERIAL){
        write_serial_char(*data);
    } else if (file == VFS_FD_STDERR){
        alert("%c", *data);
    }

}

fs_node_t* vfs_search_node(fs_node_t* parent, const char* path, size_t index_child){
	if (parent == NULL){
		return NULL;
	}
  log(LOG_SERIAL, false, "TEST VFS\n");
	fs_node_t* child;
	//size_t index_child = 1;
	while ((child = readdir_fs(parent, index_child)) != NULL){
    //log(LOG_SERIAL, false, "strcmp child->name %s path %s : %d\n", child->name, path, strcmp(child->name, path));
    //log(LOG_SERIAL, false, "readdir_fs(fs_get_root_node(), 3)->name %s\n", readdir_fs(fs_get_root_node(), 3)->name);
    //log(LOG_SERIAL, false, "index_child : %d\n", index_child);
		if (strcmp(child->name, path) == 0){
      log(LOG_SERIAL, false, "return child\n");
			return child;
		}
		if (child->node_type == FT_Directory){
      log(LOG_SERIAL, false, "directory\n");
      index_child++;
			fs_node_t* recursive_child = vfs_search_node(child, path, index_child);
			if (recursive_child != NULL){
				return recursive_child;
			}
		} else {
		++index_child;
    }
	}
	return NULL;
}

fs_node_t* vfs_open(const char* path, const char* mode){
  fs_node_t* root  = fs_get_root_node();
  if (root == NULL){
    log(LOG_SERIAL, false, "Couldn't find root node\n");
    return NULL;
  }
	fs_node_t* file = vfs_search_node(root, path, 0);
	if (file == NULL){
    log(LOG_SERIAL, false, "Couldn't find file vfs_open\n");
		return NULL;
	}
	if (file->node_type != FT_File){
		return NULL;
	}
	int read = 0;
	int write = 0;
	if (strcmp(mode, "r") == 0){
		read = 1;
	}
	if (strcmp(mode, "w") == 0){
		write = 1;
	}
	open_fs(file, read, write);
	return file;
}

