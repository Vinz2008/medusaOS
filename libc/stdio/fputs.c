#include <stdio.h>

int fputs(const char* string, file_descriptor_t file){
	return fprintf("%s\n", string);
}
