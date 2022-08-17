#include <kernel/initrd.h>
#include <stdio.h>
#include <kernel/serial.h>

struct tar_header *headers[32];

unsigned int getsize(const char *in){
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1; 
    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
    return size;
}

unsigned int initrd_get_number_files(unsigned int address){
    unsigned int i;
    for (i = 0; ; i++){
        struct tar_header *header = (struct tar_header *)address;
        if (header->filename[0] == '\0')
            break;
        write_serialf("filename %i : %s\n",i, header->filename);
        unsigned int size = getsize(header->size);
        headers[i] = header;
        address += ((size / 512) + 1) * 512;
        if (size % 512)
            address += 512;
    }
    return i;
}


void initrd_list_filenames(unsigned int address){
    unsigned int i = 0;
    while (i <= initrd_get_number_files(address)){
        struct tar_header *header = (struct tar_header *)address;
        if (header->filename[0] == '\0')
            break;
        printf("\nfilename [%i] : %s\n",i, header->filename);
        unsigned int size = getsize(header->size);
        headers[i] = header;
        address += ((size / 512) + 1) * 512;
        if (size % 512)
            address += 512;
        i++;
    }
}
