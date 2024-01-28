#include <fnctl.h>
#include <kernel/kmalloc.h>
#include <kernel/list.h>
#include <kernel/pair.h>
#include <kernel/serial.h>
#include <kernel/vfs.h>
#include <stdio.h>
#include <string.h>

#define MAX_SYMLINK_DEPTH 10

fs_node_t* fs_root = NULL;
vfs_tree_t* fs_tree = NULL;

uint32_t vfs_get_depth(const char* path);

path_t* parse_path(char* path_string) {
  path_t* path = kmalloc(sizeof(path_t));
  path->folders = list_create();
  path->path_string = strdup(path_string);
  char* temp_path = kmalloc(sizeof(char) * 4096);
  bool is_first_slash = true;
  for (int i = 0; i < strlen(path_string); i++) {
    // log(LOG_SERIAL, false, "path_string[i]  : %c\n", path_string[i]);
    if (path_string[i] == '/') {
      if (!is_first_slash) {
        log(LOG_SERIAL, false, "appending path %s\n", temp_path);
        list_append(strdup(temp_path), path->folders);
      } else {
        is_first_slash = false;
      }
      memset(temp_path, 0, 4096);
    } else {
      append(temp_path, path_string[i]);
    }
  }
  kfree(temp_path);
  return path;
}

vfs_tree_node_t* vfs_tree_node_create(char* name, bool is_folder) {
  vfs_tree_node_t* tree_node = kmalloc(sizeof(vfs_tree_node_t));
  tree_node->name = name;
  tree_node->is_mount_point = false;
  // replace NULL with fs_node for in memory folder if it is one (maybe add a
  // boolean in args)
  tree_node->mount_point_fs_node = NULL;
  // tree_node->fs_node = fs_node;
  tree_node->childrens = list_create();
  tree_node->is_folder = is_folder;
  return tree_node;
}

vfs_tree_t* vfs_tree_init() {
  vfs_tree_t* tree = kmalloc(sizeof(vfs_tree_t));
  tree->root = vfs_tree_node_create("/", true);
  // tree->root = fs_root;
  return tree;
}

void vfs_tree_set_root(vfs_tree_t* fs_tree, struct vfs_entry* vfs_node) {
  vfs_tree_node_t* root = vfs_tree_node_create("", true /*vfs_node*/);
  fs_tree->root = root;
  // fs_tree->nodes = 1;
}

vfs_tree_node_t* vfs_tree_find_child_with_name(char* name, list_t* childrens) {
  for (int i = 0; i < childrens->used; i++) {
    vfs_tree_node_t* children_temp = (vfs_tree_node_t*)childrens->list[i].data;
    if (strcmp(name, children_temp->name) == 0) {
      return children_temp;
    }
  }
  return NULL;
}

void* vfs_mount(const char* path, fs_node_t* local_root) {
  if (*path == '/') {
    log(LOG_SERIAL, false, "vfs_mount mounting root\n");
    fs_tree->root->mount_point_fs_node = local_root;
    fs_tree->root->is_mount_point = true;
  } else {
    vfs_tree_node_t* node_where_to_mount = NULL;
    node_where_to_mount = fs_tree->root;
    path_t* path_parsed = parse_path(path);
    // find node in tree where to mount
    for (int i = 0; i < path_parsed->folders->used; i++) {
      char* name = (char*)path_parsed->folders->list[i].data;
      if (!node_where_to_mount->is_folder) {
        return NULL;
      }
      vfs_tree_node_t* temp_node =
          vfs_tree_find_child_with_name(name, node_where_to_mount->childrens);
      if (temp_node) {
        node_where_to_mount = temp_node;
      } else {
        // create node
        vfs_tree_node_t* new_node = vfs_tree_node_create(name, true);
        list_append(new_node, node_where_to_mount->childrens);
        node_where_to_mount = new_node;
      }
    }
    // now node_where_to_mount should contain to the node where we will mount
    // the filesystem

    node_where_to_mount->is_mount_point = true;
    node_where_to_mount->mount_point_fs_node = local_root;

    /*if (vfs_get_depth(path) > 2){
        // Can't mount in subfolder, only in direct folder to root
        return NULL;
    }
    bool mount_folder_exists = false;
    int pos = -1;
    for (int i = 0; i < fs_tree->root->childrens->used; i++){
        if
    (strcmp(((vfs_tree_node_t*)fs_tree->root->childrens->list[i].data)->fs_node->name,
    path) == 0){ pos = i; mount_folder_exists = true;
        }
    }
    ((vfs_tree_node_t*)fs_tree->root->childrens->list[pos].data)->fs_node =
    local_root; // maybe replace entire vfs_tree_node by passing to this
    function entire vfs_tree_node or find other way to do it
    // TODO : verify if mount folder exists
    */
  }
}

// function find local root for fs_node
struct vfs_tree_find_result* vfs_tree_find_local_root(char* path) {
  if (*path == '/') {
    struct vfs_tree_find_result* result =
        kmalloc(sizeof(struct vfs_tree_find_result));
    result->path_to_use = "/";
    result->fs_node = fs_tree->root->mount_point_fs_node;
    return result;
  }
  vfs_tree_node_t* node_local_root = NULL;
  node_local_root = fs_tree->root;
  path_t* path_parsed = parse_path(path);
  for (int i = 0; i < path_parsed->folders->used; i++) {
    if (node_local_root->is_mount_point) {
      char* path_to_use = kmalloc(sizeof(char) * 4096);
      for (int j = i; j < path_parsed->folders->used; j++) {
        strcat(path_to_use, (char*)path_parsed->folders->list[j].data);
      }
      struct vfs_tree_find_result* result =
          kmalloc(sizeof(struct vfs_tree_find_result));
      result->path_to_use = path_to_use;
      result->fs_node = node_local_root->mount_point_fs_node;
      return result;
      // return (struct vfs_tree_find_result){path_to_use,
      // node_local_root->mount_point_fs_node};
    }
    char* name = (char*)path_parsed->folders->list[i].data;
    vfs_tree_node_t* temp_node =
        vfs_tree_find_child_with_name(name, node_local_root->childrens);
    if (temp_node) {
      node_local_root = temp_node;
    } else {
      // couldn't find child with this name
      return NULL;
    }
  }
  return NULL;
}

char* canonicalize_path(const char* cwd, const char* input) {
  /* This is a stack-based canonicalizer; we use a list as a stack */
  // list_t *out = list_create("vfs canonicalize_path working memory",input);
  int nb_allocated = 0;
  char** out = kmalloc(sizeof(char*) * nb_allocated++);
  int nb_out_used = 0;

  /*
   * If we have a relative path, we need to canonicalize
   * the working directory and insert it into the stack.
   */
  if (strlen(input) && input[0] != PATH_SEPARATOR) {
    /* Make a copy of the working directory */
    char* path = kmalloc((strlen(cwd) + 1) * sizeof(char));
    memcpy(path, cwd, strlen(cwd) + 1);

    /* Setup tokenizer */
    char* pch;
    char* save;
    pch = strtok_r(path, PATH_SEPARATOR_STRING, &save);

    /* Start tokenizing */
    while (pch != NULL) {
      /* Make copies of the path elements */
      char* s = kmalloc(sizeof(char) * (strlen(pch) + 1));
      memcpy(s, pch, strlen(pch) + 1);
      /* And push them */
      out[nb_out_used] = strdup(s);
      krealloc(out, sizeof(char) * nb_allocated++);
      nb_out_used++;
      pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
    }
    kfree(path);
  }

  /* Similarly, we need to push the elements from the new path */
  char* path = kmalloc((strlen(input) + 1) * sizeof(char));
  memcpy(path, input, strlen(input) + 1);

  /* Initialize the tokenizer... */
  char* pch;
  char* save;
  pch = strtok_r(path, PATH_SEPARATOR_STRING, &save);

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
                  // Regular path, push it XXX: Path elements should be checked
  for existence! char * s = malloc(sizeof(char) * (strlen(pch) + 1)); memcpy(s,
  pch, strlen(pch) + 1); list_insert(out, s);
          }
          pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
  }*/
  kfree(path);

  /* Calculate the size of the path string */
  size_t size = 0;
  for (int i = 0; i < nb_out_used; i++) {
    size += strlen(out[i]) + 1;
  }

  /* join() the list */
  char* output = kmalloc(sizeof(char) * (size + 1));
  char* output_offset = output;
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
  for (int i = 0; i < nb_out_used; i++) {
    kfree(out[i]);
  }
  kfree(out);

  /* And return a working, absolute path */
  return output;
}

/*fs_node_t* get_mount_point(char* path, uint32_t path_depth, char **outpath,
unsigned int * outdepth){ size_t depth; for (depth = 0; depth < path_depth;
++depth){ path += strlen(path) + 1;
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
}*/

/*fs_node_t* kopen_recur(const char* filename, int flags, uint64_t
symlink_depth, char* relative_to){ if (!filename){ return NULL;
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
                // Find each PATH_SEPARATOR
                if (*path_offset == PATH_SEPARATOR) {
                        *path_offset = '\0';
                        path_depth++;
                }
                path_offset++;
        }
        // Clean up
        path[path_len] = '\0';
        path_offset = path + 1;

        // At this point, the path is tokenized and path_offset points to the
first token (directory) and path_depth is the number of directories in the path
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
                        // We found the file and are done, open the node
                        open_fs(node_ptr, flags);
                        kfree(path);
                        return node_ptr;
                }
                // We are still searching...
                log(LOG_SERIAL, false, "... Searching for %s", path_offset);
                fs_node_t * node_next = finddir_fs(node_ptr, path_offset);
                kfree(node_ptr); // Always a clone or an unopened thing
                node_ptr = node_next;
                // Search the active directory for the requested directory
                if (!node_ptr) {
                        // We failed to find the requested directory
                        kfree(path);
                        return NULL;
                }
                path_offset += strlen(path_offset) + 1;
                ++depth;
        } while (depth < path_depth + 1);
        log(LOG_SERIAL, false, "- Not found.");
        // We failed to find the requested file, but our loop terminated.
        kfree(path);
        return NULL;
}*/

/*
fs_node_t *kopen(const char *filename, unsigned int flags) {
        log(LOG_SERIAL, false, "kopen(%s)", filename);

        //return kopen_recur(filename, flags, 0, (char
*)(this_core->current_process->wd_name)); return kopen_recur(filename, flags, 0,
calloc(10, sizeof(char)));
}*/

void vfs_init() {
  fs_tree = vfs_tree_init();
  // struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));
  // root->name = strdup("[root]");
  // root->file = NULL;
  // root->fs_type = NULL;
  // root->device = NULL;
  // vfs_tree_set_root(fs_tree, root);
  init_file_descriptor_table();
}

/*void* vfs_mount(const char* path, fs_node_t* local_root){
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
        vfs_tree_node_t* tchild =
(vfs_tree_node_t*)node->children_list->childrens[i].value;
      }
    }
  }
  return ret_val;
}*/

int get_file_size_fs(fs_node_t* node) {
  if (node == NULL) {
    log(LOG_SERIAL, false, "NULL in get_file_size_fs\n");
    return -1;
  }
  if (node->get_file_size != 0) {
    return node->get_file_size(node);
  } else {
    return -1;
  }
}

uint32_t read_fs(fs_node_t* node, uint32_t offset, uint32_t size,
                 uint8_t* buffer) {
  // Has the node got a read callback?
  if (node == NULL) {
    log(LOG_SERIAL, false, "NULL in read_fs\n");
    return;
  }
  if (node->read != 0)
    return node->read(node, offset, size, buffer);
  else
    return 0;
}

void open_fs(fs_node_t* node, uint32_t flags) {
  // Has the node got a read callback?
  if (node->open != 0)
    return node->open(node);
  else {
    node->mode = flags;
  }
}

void close_fs(fs_node_t* node) {
  // Has the node got a read callback?
  if (node == NULL) {
    return;
  }
  if (node->close != 0)
    return node->close(node);
  else
    node->mode = 0;
  // return;
}

uint32_t write_fs(fs_node_t* node, uint32_t offset, uint32_t size,
                  uint8_t* buffer) {
  // Has the node got a read callback?
  if (node->write != 0)
    return node->write(node, offset, size, buffer);
  else
    return 0;
}

// TODO : remove index at some point in args or create wrapper to be compatible
// with linux readdir
struct dirent* readdir_fs(fs_node_t* node, uint32_t index) {
  // Is the node a directory, and does it have a callback?
  /*if ( (node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0 )
          return node->readdir(node, index);
  else
          return 0;*/
  if (node->readdir == 0) {
    return 0;
  }
  struct dirent* dir = node->readdir(node, index);
  if (strcmp("dev", dir->name) == 0) {
    log(LOG_SERIAL, false, "DEV FOUND\n");
    return dir;
  } else {
    return dir;
  }
}

fs_node_t* finddir_fs(fs_node_t* node, char* name) {
  // Is the node a directory, and does it have a callback?
  if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0)
    return node->finddir(node, name);
  else
    return 0;
}

fs_node_t* fs_get_root_node() { return fs_root; }

uint32_t vfs_get_depth(const char* path) {
  uint32_t depth = 0;

  for (size_t i = 0; path[i] != '\0'; ++i) {
    if (path[i] == '/')
      ++depth;
  }

  return depth;
}

list_t* file_descriptors_table;

file_descriptor_t stdinfd;
file_descriptor_t stdoutfd;
file_descriptor_t stderrfd;
file_descriptor_t serialfd;

void init_file_descriptor_table() {
  file_descriptors_table = list_create();
  // should have struct to have pairs of file descriptors and fs_node pointers
  list_append(create_pair_a(&stdinfd, vfs_open("/dev/stdin", "r")),
              file_descriptors_table);
  list_append(&stdoutfd, file_descriptors_table);
  list_append(&stderrfd, file_descriptors_table);
  list_append(&serialfd, file_descriptors_table);
  char* path_test = "/dev/proc/a/b/c";
  path_t* path = parse_path(path_test);
  log(LOG_SERIAL, false, "path parsing test\n");
  for (int i = 0; i < path->folders->used; i++) {
    log(LOG_SERIAL, false, "path %d : %s\n", i,
        (char*)path->folders->list[i].data);
  }
}

// write a character
int vfs_write_fd(file_descriptor_t file, uint8_t* data, size_t size) {
  (void)size;
  if (file == VFS_FD_STDOUT) {
    putchar(*data);
  } else if (file == VFS_FD_SERIAL) {
    write_serial_char(*data);
  } else if (file == VFS_FD_STDERR) {
    alert("%c", *data);
  }
}

#if VFS_NEW_IMPL

fs_node_t* vfs_search_node(fs_node_t* local_root, const char* path,
                           size_t index_child) {
  if (!local_root) {
    return NULL;
  }
  fs_node_t* child;
  while ((child = readdir_fs(local_root, index_child)) != NULL) {
    if (strcmp(child->name, path) == 0) {
      return child;
    }
    if (child->node_type == FT_Directory) {
      index_child++;
      fs_node_t* recursive_child = vfs_search_node(child, path, index_child);
      if (recursive_child != NULL) {
        return recursive_child;
      }
    } else {
      ++index_child;
    }
  }
  return NULL;
}

#else

fs_node_t* vfs_search_node(fs_node_t* parent, const char* path,
                           size_t index_child) {
  if (parent == NULL) {
    return NULL;
  }
  log(LOG_SERIAL, false, "TEST VFS\n");
  fs_node_t* child;
  // size_t index_child = 1;
  while ((child = readdir_fs(parent, index_child)) != NULL) {
    // log(LOG_SERIAL, false, "strcmp child->name %s path %s : %d\n",
    // child->name, path, strcmp(child->name, path)); log(LOG_SERIAL, false,
    // "readdir_fs(fs_get_root_node(), 3)->name %s\n",
    // readdir_fs(fs_get_root_node(), 3)->name); log(LOG_SERIAL, false,
    // "index_child : %d\n", index_child);
    if (strcmp(child->name, path) == 0) {
      log(LOG_SERIAL, false, "return child\n");
      return child;
    }
    if (child->node_type == FT_Directory) {
      log(LOG_SERIAL, false, "directory %s\n", child->name);
      index_child++;
      fs_node_t* recursive_child = vfs_search_node(child, path, index_child);
      if (recursive_child != NULL) {
        return recursive_child;
      }
    } else {
      ++index_child;
    }
  }
  return NULL;
}

#endif

#if VFS_NEW_IMPL

fs_node_t* vfs_open(const char* path, const char* mode) {
  log(LOG_SERIAL, false, "vfs_open %s\n", path);
  struct vfs_tree_find_result* result = vfs_tree_find_local_root(path);
  if (!result) {
    return NULL;
  }
  fs_node_t* local_root = result->fs_node;
  char* path_to_use = result->path_to_use;
  fs_node_t* fs_node = vfs_search_node(local_root, path_to_use, 0);

  if (!fs_node) {
    return NULL;
  }

  /*struct vfs_tree_find_result* result = vfs_tree_find_local_root(path);
  if (!result){
      return NULL;
  }
  fs_node_t* local_root = result->fs_node;
  char* path_to_use = result->path_to_use;
  while ()


  kfree(result);*/

  // fs_node_t* fs_node = NULL;
  uint32_t flags = 0;
  if (strcmp(mode, "r") == 0) {
    flags |= O_RDONLY;
  }
  if (strcmp(mode, "w") == 0) {
    flags |= O_WRONLY;
  }
  open_fs(fs_node, flags);
  return fs_node;
}

#else

fs_node_t* vfs_open(const char* path, const char* mode) {
  fs_node_t* root = fs_get_root_node();
  if (root == NULL) {
    log(LOG_SERIAL, false, "Couldn't find root node\n");
    return NULL;
  }
  fs_node_t* file = vfs_search_node(root, path, 0);
  if (file == NULL) {
    log(LOG_SERIAL, false, "Couldn't find %s file vfs_open\n", path);
    return NULL;
  }
  if (file->node_type != FT_File) {
    return NULL;
  }
  uint32_t flags = 0;
  if (strcmp(mode, "r") == 0) {
    flags |= O_RDONLY;
  }
  if (strcmp(mode, "w") == 0) {
    flags |= O_WRONLY;
  }
  open_fs(file, flags);
  return file;
}

#endif
