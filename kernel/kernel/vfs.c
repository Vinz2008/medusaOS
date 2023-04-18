#include <kernel/vfs.h>
#include <stdio.h>
#include <string.h>
#include <fnctl.h>
#include <kernel/kmalloc.h>
#include <kernel/serial.h>
#include <kernel/list.h>
#include <kernel/pair.h>


#define MAX_SYMLINK_DEPTH 10

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
  temp->value = vfs_node;
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

char *canonicalize_path(const char *cwd, const char *input) {
	/* This is a stack-based canonicalizer; we use a list as a stack */
	//list_t *out = list_create("vfs canonicalize_path working memory",input);
  int nb_allocated = 0;
  char** out = kmalloc(sizeof(char*) * nb_allocated++);
  int nb_out_used = 0;

	/*
	 * If we have a relative path, we need to canonicalize
	 * the working directory and insert it into the stack.
	 */
	if (strlen(input) && input[0] != PATH_SEPARATOR) {
		/* Make a copy of the working directory */
		char *path = kmalloc((strlen(cwd) + 1) * sizeof(char));
		memcpy(path, cwd, strlen(cwd) + 1);

		/* Setup tokenizer */
		char *pch;
		char *save;
		pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

		/* Start tokenizing */
		while (pch != NULL) {
			/* Make copies of the path elements */
			char *s = kmalloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			/* And push them */
      out[nb_out_used] = strdup(s);
      krealloc(out, sizeof(char) * nb_allocated++);
      nb_out_used++;
			pch = strtok_r(NULL,PATH_SEPARATOR_STRING,&save);
		}
		kfree(path);
	}

	/* Similarly, we need to push the elements from the new path */
	char *path = kmalloc((strlen(input) + 1) * sizeof(char));
	memcpy(path, input, strlen(input) + 1);

	/* Initialize the tokenizer... */
	char *pch;
	char *save;
	pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

	/*
	 * Tokenize the path, this time, taking care to properly
	 * handle .. and . to represent up (stack pop) and current
	 * (do nothing)
	 */
  
	/*while (pch != NULL) {
		if (!strcmp(pch,PATH_UP)) {
			// Path = ..  Pop the stack to move up a directory
			node_t * n = list_pop(out);
			if (n) {
				free(n->value);
				free(n);
			}
		} else if (!strcmp(pch,PATH_DOT)) {
			//Path = . Do nothing
		} else {
			// Regular path, push it XXX: Path elements should be checked for existence!
			char * s = malloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			list_insert(out, s);
		}
		pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
	}*/
	kfree(path);

	/* Calculate the size of the path string */
	size_t size = 0;
  for (int i = 0; i < nb_out_used; i++){
    size += strlen(out[i]) + 1;
  }

	/* join() the list */
	char *output = kmalloc(sizeof(char) * (size + 1));
	char *output_offset = output;
	if (size == 0) {
		/*
		 * If the path is empty, we take this to mean the root
		 * thus we synthesize a path of "/" to return.
		 */
		output = krealloc(output, sizeof(char) * 2);
		output[0] = PATH_SEPARATOR;
		output[1] = '\0';
	} else {
		/* Otherwise, append each element together */

		for (int i = 0; i < nb_out_used; i++) {
			output_offset[0] = PATH_SEPARATOR;
			output_offset++;
			memcpy(output_offset, out[i], strlen(out[i]) + 1);
			output_offset += strlen(out[i]);
		}
	}

	/* Clean up the various things we used to get here */
  for (int i = 0; i < nb_out_used; i++){
    kfree(out[i]);
  }
  kfree(out);

	/* And return a working, absolute path */
	return output;
}

fs_node_t* get_mount_point(char* path, uint32_t path_depth, char **outpath, unsigned int * outdepth){
  size_t depth;
  for (depth = 0; depth < path_depth; ++depth){
    path += strlen(path) + 1;
  }
  fs_node_t* last = fs_root;
  vfs_tree_node_t* node = fs_tree->root;
  char* at = *outpath;
  int _depth = 1;
	int _tree_depth = 0;
  while (1){
    if (at >= path){
      break;
    }
    int found = 0;
    for (int i = 0; i < node->children_list->used; i++){
      struct vfs_children* child = &node->children_list->childrens[i];
      vfs_tree_node_t* tchild = (vfs_tree_node_t *)child->value;
      struct vfs_entry * ent = (struct vfs_entry *)tchild->value;
      if (!strcmp(ent->name, at)) {
				found = 1;
				node = tchild;
				at = at + strlen(at) + 1;
				if (ent->file) {
					_tree_depth = _depth;
					last = ent->file;
					*outpath = at;
				}
				break;
			}
    }
    if (!found){
      break;
    }
    _depth++;
  }
  *outdepth = _tree_depth;
  if (last){
    fs_node_t* last_clone = kmalloc(sizeof(fs_node_t));
    memcpy(last_clone, last, sizeof(fs_node_t));
    return last_clone;
  }
  return last;
}

fs_node_t* kopen_recur(const char* filename, int flags, uint64_t symlink_depth, char* relative_to){
  if (!filename){
    return NULL;
  }
  char *path = canonicalize_path(relative_to, filename);
  size_t path_len = strlen(path);
  if (path_len == 1){
    fs_node_t *root_clone = kmalloc(sizeof(fs_node_t));
    memcpy(root_clone, fs_root, sizeof(fs_node_t));
    kfree(path);
    open_fs(root_clone, flags);
    return root_clone;
  }
  char *path_offset = path;
  uint64_t path_depth = 0;
  while (path_offset < path + path_len) {
		/* Find each PATH_SEPARATOR */
		if (*path_offset == PATH_SEPARATOR) {
			*path_offset = '\0';
			path_depth++;
		}
		path_offset++;
	}
	/* Clean up */
	path[path_len] = '\0';
	path_offset = path + 1;
  /*
	 * At this point, the path is tokenized and path_offset points
	 * to the first token (directory) and path_depth is the number
	 * of directories in the path
	 */
  unsigned int depth = 0;
  fs_node_t *node_ptr = get_mount_point(path, path_depth, &path_offset, &depth);
  log(LOG_SERIAL, false, "path offset : %s\n", path_offset);
  log(LOG_SERIAL, false, "depth : %s\n", depth);
  if (!node_ptr) return NULL;
  do {

		if (path_offset >= path+path_len) {
			kfree(path);
			open_fs(node_ptr, flags);
			return node_ptr;
		}
		if (depth == path_depth) {
			/* We found the file and are done, open the node */
			open_fs(node_ptr, flags);
			kfree(path);
			return node_ptr;
		}
		/* We are still searching... */
		log(LOG_SERIAL, false, "... Searching for %s", path_offset);
		fs_node_t * node_next = finddir_fs(node_ptr, path_offset);
		kfree(node_ptr); /* Always a clone or an unopened thing */
		node_ptr = node_next;
		/* Search the active directory for the requested directory */
		if (!node_ptr) {
			/* We failed to find the requested directory */
			kfree(path);
			return NULL;
		}
		path_offset += strlen(path_offset) + 1;
		++depth;
	} while (depth < path_depth + 1);
	log(LOG_SERIAL, false, "- Not found.");
	/* We failed to find the requested file, but our loop terminated. */
	kfree(path);
	return NULL;
}

fs_node_t *kopen(const char *filename, unsigned int flags) {
	log(LOG_SERIAL, false, "kopen(%s)", filename);

	return kopen_recur(filename, flags, 0, /*(char *)(this_core->current_process->wd_name)*/calloc(10, sizeof(char)));
}


void vfs_init(){
  fs_tree = vfs_tree_init();
  struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));
  root->name = strdup("[root]");
  root->file = NULL;
  root->fs_type = NULL;
  root->device = NULL;
  vfs_tree_set_root(fs_tree, root);
  init_file_descriptor_table();
}

void* vfs_mount(const char* path, fs_node_t* local_root){
  vfs_tree_node_t* ret_val = NULL;
  char* p = strdup(path);
  vfs_tree_node_t* root_node = fs_tree->root;
  if (*path == '/'){
    struct vfs_entry* root = (struct vfs_entry*)root_node->value;
    if (root->file){
      log(LOG_SERIAL, false, "Path %s already mounted\n", path);
    }
    root->file = local_root;

  } else {
    vfs_tree_node_t* node = root_node;
    while (1){
      int found = 0;
      for (int i = 0; i < node->children_list->used; i++){
        vfs_tree_node_t* tchild = (vfs_tree_node_t*)node->children_list->childrens[i].value;
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

void open_fs(fs_node_t *node, uint32_t flags){
  // Has the node got a read callback?
  if (node->open != 0)
    return node->open(node);
  else {
    node->mode =  flags;
    /*if (read == 1){
      node->mode =  READ_MODE;
    } else if (write == 1){
      node->mode = WRITE_MODE;
    }*/
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

list_t* file_descriptors_table;

file_descriptor_t stdinfd;
file_descriptor_t stdoutfd;
file_descriptor_t stderrfd;
file_descriptor_t serialfd;

void init_file_descriptor_table(){
  file_descriptors_table = list_create();
  // should have struct to have pairs of file descriptors and fs_node pointers
  list_append(create_pair_a(&stdinfd, vfs_open("/dev/stdin", "r")), file_descriptors_table);
  list_append(&stdoutfd, file_descriptors_table);
  list_append(&stderrfd, file_descriptors_table);
  list_append(&serialfd, file_descriptors_table);
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
  uint32_t flags = 0;
	if (strcmp(mode, "r") == 0){
    flags |= O_RDONLY;
	}
	if (strcmp(mode, "w") == 0){
		flags |= O_WRONLY;
	}
	open_fs(file, flags);
	return file;
}



