#include <kernel/vfs.h>
#include <stdio.h>
#include <string.h>
#include <kernel/serial.h>

fs_node_t *fs_root = NULL;

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
  return node->readdir(node, index);
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
    log(LOG_SERIAL, false, "strcmp child->name %s path %s : %d\n", child->name, path, strcmp(child->name, path));
    log(LOG_SERIAL, false, "readdir_fs(fs_get_root_node(), 3)->name %s\n", readdir_fs(fs_get_root_node(), 3)->name);
    log(LOG_SERIAL, false, "index_child : %d\n", index_child);
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

