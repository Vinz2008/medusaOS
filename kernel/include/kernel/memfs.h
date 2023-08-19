
// could at some point use it for other in memory filesystems and rename it to in_mem_file or in_memory_file
struct memfs_file {
	const char* name;
	char* file_content;
	bool is_folder;
}
