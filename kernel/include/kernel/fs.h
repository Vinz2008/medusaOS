#include "vfs.h"

struct file_system_type {
    const char* name;
    fs_node_t* (*mount)();
};
